// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <exception>

#include <mmtf/structure_data.hpp>
#include <mmtf/decoder.hpp>
#include <mmtf/encoder.hpp>
#include <mmtf/export_helpers.hpp>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/Connectivity.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/formats/MMTF.hpp"

using namespace chemfiles;

static int8_t bond_order_to_mmtf(Bond::BondOrder order);
static Bond::BondOrder bond_order_to_chemfiles(int32_t order);
static void set_secondary(Residue& residue, int32_t code);

template<> FormatInfo chemfiles::format_information<MMTFFormat>() {
    return FormatInfo("MMTF").with_extension(".mmtf").description(
        "MMTF (RCSB Protein Data Bank) binary format"
    );
}

MMTFFormat::MMTFFormat(std::string path, File::Mode mode, File::Compression compression) {
    if (mode == File::READ) {
        auto file = TextFile(std::move(path), mode, compression);
        auto buffer = file.readall();
        mmtf::decodeFromBuffer(structure_, buffer.data(), buffer.size());
        if (!structure_.hasConsistentData()) {
            throw format_error("issue with: {}. Please ensure it is valid MMTF file", path);
        }
    } else if (mode == File::WRITE) {
        filename_ = path; // We really don't need to do anything, yet
    } else if (mode == File::APPEND) {
        throw file_error("append mode ('a') is not supported for the MMTF format");
    }
}

MMTFFormat::~MMTFFormat() {
    if (!filename_.empty()) {
        try {
            mmtf::compressGroupList(structure_);
            encodeToFile(structure_, filename_);
        } catch (const std::exception& e) {
            warning("[MMTF writer] error while finishing writing to {}: {}", filename_, e.what());
        } catch (...) {
            // ignore exceptions in destructor
        }
    }
}

size_t MMTFFormat::nsteps() {
    return static_cast<size_t>(structure_.numModels);
}

void MMTFFormat::read_step(const size_t step, Frame& frame) {
    modelIndex_ = 0;
    chainIndex_ = 0;
    groupIndex_ = 0;
    atomIndex_  = 0;
    atomSkip_   = 0;

    // Fast-forward, keeping all indexes updated
    while(modelIndex_ != step) {
        auto chainsPerModel = static_cast<size_t>(structure_.chainsPerModel[modelIndex_]);
        while(chainIndex_ != chainsPerModel) {
            auto groupsPerChain = static_cast<size_t>(structure_.groupsPerChain[chainIndex_]);
            while(groupIndex_ != groupsPerChain) {
                auto groupType = static_cast<size_t>(structure_.groupTypeList[groupIndex_]);
                auto group = structure_.groupList[groupType];
                auto atomCount = group.atomNameList.size();
                atomIndex_ += atomCount;
                groupIndex_++;
            }
            groupIndex_ = 0;
            chainIndex_++;
        }
        chainIndex_ = 0;
        modelIndex_++;
    }

    atomSkip_ = atomIndex_;

    read(frame);
}

void MMTFFormat::read(Frame& frame) {
    if (structure_.unitCell.size() == 6) {
        frame.set_cell({
            static_cast<double>(structure_.unitCell[0]),
            static_cast<double>(structure_.unitCell[1]),
            static_cast<double>(structure_.unitCell[2]),
            static_cast<double>(structure_.unitCell[3]),
            static_cast<double>(structure_.unitCell[4]),
            static_cast<double>(structure_.unitCell[5])
        });
    }

    if (!mmtf::isDefaultValue(structure_.title)) {
        frame.set("name", structure_.title);
    }

    if (!mmtf::isDefaultValue(structure_.structureId)) {
        frame.set("pdb_idcode", structure_.structureId);
    }

    if (!mmtf::isDefaultValue(structure_.depositionDate)) {
        frame.set("deposition_date", structure_.depositionDate);
    }

    auto inter_residue_bond_count = structure_.bondAtomList.size() / 2;
    size_t bond_index = 0;
    while (bond_index < inter_residue_bond_count) {
        auto atom1 = structure_.bondAtomList[bond_index * 2 + 0];
        auto atom2 = structure_.bondAtomList[bond_index * 2 + 1];

        // We are below the atoms we care about
        if ((atom1 < atomSkip_) || (atom2 < atomSkip_)) {
            bond_index++;
            continue;
        }

        break;
    }

    // count the number of atoms in this frame/model
    size_t natoms = 0;
    auto modelChainCount = static_cast<size_t>(structure_.chainsPerModel[modelIndex_]);
    // backup chainIndex & groupIndex to be incremented in the loop below
    auto chain_index = chainIndex_;
    auto group_index = groupIndex_;
    for (size_t j = 0; j < modelChainCount; j++) {
        auto chainGroupCount = static_cast<size_t>(structure_.groupsPerChain[chain_index]);
        for (size_t k = 0; k < chainGroupCount; k++) {
            auto groupType = static_cast<size_t>(structure_.groupTypeList[group_index]);
            const auto& group = structure_.groupList[groupType];
            natoms += group.atomNameList.size();
            group_index++;
        }
        chain_index++;
    }

    frame.resize(natoms);
    auto positions = frame.positions();
    // Read the structure iterating over the chains in the model, then the
    // residues/groups in the chain and finally the atoms in the residue/group
    for (size_t j = 0; j < modelChainCount; j++) {
        auto chainGroupCount = static_cast<size_t>(structure_.groupsPerChain[chainIndex_]);

        // Unfortunetly we must loop through the assembly lists to find which
        // one our current chain belongs to. Forunetly, these lists are fairly
        // short in the vast majority of cases.
        std::string current_assembly;
        for (const auto& assembly : structure_.bioAssemblyList) {
            for (const auto& transform : assembly.transformList) {
                for (auto id : transform.chainIndexList) {
                    if (static_cast<size_t>(id) == chainIndex_) {
                        current_assembly += "bio";
                        current_assembly += assembly.name;
                    }
                }
            }
        }

        // A group is like a residue or other molecule in a PDB file.
        for (size_t k = 0; k < chainGroupCount; k++) {
            auto groupType = static_cast<size_t>(structure_.groupTypeList[groupIndex_]);
            const auto& group = structure_.groupList[groupType];

            auto groupId = static_cast<size_t>(structure_.groupIdList[groupIndex_]);
            auto residue = Residue(group.groupName, groupId);
            residue.set("composition_type", group.chemCompType);
            residue.set("is_standard_pdb", !mmtf::is_hetatm(group.chemCompType.c_str()));

            // map between the index of the atom in this group and the global
            // index in the frame. This is needed to add bonds from this group
            std::vector<size_t> global_indexes;
            global_indexes.reserve(group.atomNameList.size());
            for (size_t l = 0; l < group.atomNameList.size(); l++) {
                auto atom = Atom(group.atomNameList[l], group.elementList[l]);
                atom.set_charge(static_cast<double>(group.formalChargeList[l]));

                const auto& altLocList = structure_.altLocList;
                if (!mmtf::isDefaultValue(altLocList) && !(
                    altLocList[atomIndex_] == ' ' ||
                    altLocList[atomIndex_] == 0x00)) {
                    atom.set("altloc", std::string(1, altLocList[atomIndex_]));
                }

                auto id = atom_id(atomIndex_);
                global_indexes.emplace_back(id);
                frame[id] = std::move(atom);
                residue.add_atom(id);

                positions[id][0] = static_cast<double>(structure_.xCoordList[atomIndex_]);
                positions[id][1] = static_cast<double>(structure_.yCoordList[atomIndex_]);
                positions[id][2] = static_cast<double>(structure_.zCoordList[atomIndex_]);

                atomIndex_++;
            }

            for (size_t l = 0; l < group.bondOrderList.size(); l++) {
                // atom1 and atom2 refer to data in group, so we need to
                // translate them to the global atomic index
                auto atom1 = static_cast<size_t>(group.bondAtomList[l * 2]);
                auto atom2 = static_cast<size_t>(group.bondAtomList[l * 2 + 1]);

                frame.add_bond(
                    global_indexes[atom1],
                    global_indexes[atom2],
                    bond_order_to_chemfiles(group.bondOrderList[l])
                );
            }

            // Add additional global (not by group) bonds
            while (bond_index < inter_residue_bond_count) {
                auto atom1 = static_cast<size_t>(structure_.bondAtomList[bond_index * 2]);
                auto atom2 = static_cast<size_t>(structure_.bondAtomList[bond_index * 2 + 1]);

                // We are below the atoms we care about
                if (atom1 < atomSkip_ || atom2 < atomSkip_) {
                    continue;
                }

                // We are above the atoms we care about
                if (atom1 > atomIndex_ || atom2 > atomIndex_) {
                    break;
                }

                frame.add_bond(atom_id(atom1), atom_id(atom2));
                bond_index++;
            }

            if (groupIndex_ < structure_.secStructList.size()) {
                set_secondary(residue, structure_.secStructList[groupIndex_]);
            }

            // If the name of the current assembly is defined in the MMTF file.
            // Bioassemblies are optional, however.
            if (!current_assembly.empty()) {
                residue.set("assembly", current_assembly);
            }

            // This is a string in MMTF, differs from the name as then increments linearly
            // For example, the fourth chainid in ( A B A B) would be D, not B (the chainname)
            residue.set("chainid", structure_.chainIdList[chainIndex_]);
            residue.set("chainindex", chainIndex_);
            if (!mmtf::isDefaultValue(structure_.chainNameList)) {
                residue.set("chainname", structure_.chainNameList[chainIndex_]);
            }

            frame.add_residue(std::move(residue));
            groupIndex_++;
        }
        chainIndex_++;
    }
    modelIndex_++;
    atomSkip_ = atomIndex_;
}

void MMTFFormat::write(const Frame& frame) {
    structure_.numModels++;
    structure_.chainsPerModel.emplace_back(0);
    structure_.numAtoms += static_cast<int32_t>(frame.size());

    const auto& topology = frame.topology();

    // pre-allocate some memory
    auto size = static_cast<size_t>(structure_.numAtoms);
    structure_.xCoordList.reserve(size);
    structure_.yCoordList.reserve(size);
    structure_.zCoordList.reserve(size);
    structure_.atomIdList.reserve(size);

    structure_.groupList.reserve(structure_.groupList.size() + topology.residues().size());

    new_atom_indexes_.clear();
    new_atom_indexes_.resize(frame.size(), -1);

    std::string previous_chainId;
    std::string previous_chainName;
    bool got_at_least_one_chain_in_this_frame = false;
    for (const auto& residue: topology.residues()) {
        // WARNING: this assumes that residues in the same chain are contiguous
        //
        // [residue[chainid="A"], residue[chainid="B"], residue[chainid="A"]]
        // will result in three chains
        const auto& chainId = residue.get<Property::STRING>("chainid").value_or("");
        const auto& chainName = residue.get<Property::STRING>("chainname").value_or("");
        if (chainName != previous_chainName ||
            chainId != previous_chainId ||
            !got_at_least_one_chain_in_this_frame
        ) {
            got_at_least_one_chain_in_this_frame = true;
            structure_.numChains++;
            structure_.chainsPerModel.back()++;

            // No residues in this chain yet
            structure_.groupsPerChain.emplace_back(0);

            structure_.chainIdList.emplace_back(chainId);
            structure_.chainNameList.emplace_back(chainName);

            previous_chainId = chainId;
            previous_chainName = chainName;
        }

        this->add_residue_to_structure(frame, residue);
    }

    // For all atoms not inside a residue, add them inside a new residue,
    // containing only this atom
    auto not_in_residue = std::vector<size_t>();
    for (size_t i=0; i<frame.size(); i++) {
        if (!topology.residue_for_atom(i)) {
            not_in_residue.emplace_back(i);
        }
    }

    if (!not_in_residue.empty()) {
        // add all of these new articficial residues in a separate chain
        structure_.numChains++;
        structure_.chainsPerModel.back()++;
        structure_.groupsPerChain.emplace_back(0);
        structure_.chainIdList.emplace_back("");
        structure_.chainNameList.emplace_back("");

        for (auto i: not_in_residue) {
            auto residue = Residue("");
            residue.add_atom(i);
            this->add_residue_to_structure(frame, residue);
        }
    }

    mmtf::BondAdder add_mmtf_bond(structure_);
    const auto& bonds = topology.bonds();
    const auto& bond_orders = topology.bond_orders();
    for (size_t i = 0; i < bonds.size(); ++i) {
        add_mmtf_bond(
            new_atom_indexes_[bonds[i][0]],
            new_atom_indexes_[bonds[i][1]],
            bond_order_to_mmtf(bond_orders[i])
        );
    }

    atomSkip_ += frame.size();
}

void MMTFFormat::add_residue_to_structure(const Frame& frame, const Residue& residue) {
    structure_.numGroups++;
    structure_.groupsPerChain.back() += 1;

    auto groupType = static_cast<int32_t>(structure_.groupList.size());
    structure_.groupTypeList.emplace_back(groupType);

    int32_t groupId = residue.id() ? static_cast<int32_t>(residue.id().value()) : -1;
    structure_.groupIdList.emplace_back(groupId);

    auto group = mmtf::GroupType();
    group.groupName = residue.name();

    auto composition_type = residue.get<Property::STRING>("composition_type").value_or("other");
    group.chemCompType = std::move(composition_type);

    // pre-allocate some memory
    group.formalChargeList.reserve(residue.size());
    group.atomNameList.reserve(residue.size());
    group.elementList.reserve(residue.size());

    auto positions = frame.positions();
    for (auto i: residue) {
        const auto& atom = frame[i];
        group.formalChargeList.emplace_back(atom.charge());

        if (atom.name().size() > 5) {
            warning("[MMTF Writer] atom name '{}' is too long for MMTF format, it will be truncated",
                atom.name()
            );
        }
        group.atomNameList.emplace_back(atom.name().substr(0, 5));

        if (atom.type().size() > 3) {
            warning("[MMTF Writer] atom type '{}' is too long for MMTF format, it will be truncated",
                atom.type()
            );
        }
        group.elementList.emplace_back(atom.type().substr(0, 3));

        new_atom_indexes_[i] = static_cast<int32_t>(structure_.xCoordList.size());
        structure_.atomIdList.emplace_back(atomSkip_ + i + 1);
        structure_.xCoordList.emplace_back(positions[i][0]);
        structure_.yCoordList.emplace_back(positions[i][1]);
        structure_.zCoordList.emplace_back(positions[i][2]);
    }
    structure_.groupList.emplace_back(std::move(group));
}

// A function to translate from the index in MMTF lists to an atom id
// suitable for chemfiles: starts at 0 for each model, and correspond to
// the initial atom index if it exists.
size_t MMTFFormat::atom_id(size_t mmtf_id) {
    if (!mmtf::isDefaultValue(structure_.atomIdList)) {
        auto id = static_cast<size_t>(structure_.atomIdList[mmtf_id]) - 1;
        assert(atomSkip_ <= id);
        return id - atomSkip_;
    } else {
        assert(atomSkip_ <= mmtf_id);
        return mmtf_id - atomSkip_;
    }
}

int8_t bond_order_to_mmtf(Bond::BondOrder order) {
    switch(order) {
    case Bond::BondOrder::SINGLE:
        return 1;
    case Bond::BondOrder::DOUBLE:
        return 2;
    case Bond::BondOrder::TRIPLE:
        return 3;
    case Bond::BondOrder::QUADRUPLE:
        return 4;
    case Bond::BondOrder::UNKNOWN:
        return -1;
    case Bond::BondOrder::QINTUPLET:
    case Bond::BondOrder::AMIDE:
    case Bond::BondOrder::AROMATIC:
    case Bond::BondOrder::UP:
    case Bond::BondOrder::DOWN:
    case Bond::BondOrder::DATIVE_L:
    case Bond::BondOrder::DATIVE_R:
        warning("[MMTF Writer] bond order '{}' can not be represented in MMTF, defaulting to single bond",
            order
        );
        return 1;
    default:
        unreachable();
    }
}

Bond::BondOrder bond_order_to_chemfiles(int32_t order) {
    switch(order) {
    case 1:
        return Bond::SINGLE;
    case 2:
        return Bond::DOUBLE;
    case 3:
        return Bond::TRIPLE;
    case 4:
        return Bond::QUADRUPLE;
    case -1:
        return Bond::UNKNOWN;
    default:
        warning("[MMTF Reader] unexpected bond order from MMTF '{}'", order);
        return Bond::UNKNOWN;
    }
}

void set_secondary(Residue& residue, int32_t code) {
    // We use the raw values here to directly match the MMTF spec
    // See https://github.com/rcsb/mmtf/blob/master/spec.md#secstructlist
    switch(code) {
    case 0:
        residue.set("secondary_structure", "pi helix");
        break;
    case 1:
        residue.set("secondary_structure", "bend");
        break;
    case 2:
        residue.set("secondary_structure", "alpha helix");
        break;
    case 3:
        residue.set("secondary_structure", "extended");
        break;
    case 4:
        residue.set("secondary_structure", "3-10 helix");
        break;
    case 5:
        residue.set("secondary_structure", "bridge");
        break;
    case 6:
        residue.set("secondary_structure", "turn");
        break;
    case 7:
        residue.set("secondary_structure", "coil");
        break;
    case -1:
        break;
    default:
        warning("[MMTF Reader] unknown secondary structure code '{}'", code);
        break;
    }
}
