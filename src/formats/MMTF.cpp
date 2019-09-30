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
                ++groupIndex_;
            }
            groupIndex_ = 0;
            ++chainIndex_;
        }
        chainIndex_ = 0;
        ++modelIndex_;
    }

    atomSkip_ = atomIndex_;

    read(frame);
}

void MMTFFormat::read(Frame& frame) {
    frame.resize(0);

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

    auto modelChainCount = static_cast<size_t>(structure_.chainsPerModel[modelIndex_]);
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

            // Save the offset before we go changing it
            size_t atomOffset = atomIndex_ - atomSkip_;

            auto groupSize = group.atomNameList.size();
            for (size_t l = 0; l < groupSize; l++) {
                auto atom = Atom(group.atomNameList[l]);

                const auto& altLocList = structure_.altLocList;
                if (!mmtf::isDefaultValue(altLocList) && !(
                    altLocList[atomIndex_] == ' ' ||
                    altLocList[atomIndex_] == 0x00)) {
                    atom.set("altloc", std::string(1, altLocList[atomIndex_]));
                }

                atom.set_type(group.elementList[l]);
                atom.set_charge(static_cast<double>(group.formalChargeList[l]));
                auto position = Vector3D(
                    static_cast<double>(structure_.xCoordList[atomIndex_]),
                    static_cast<double>(structure_.yCoordList[atomIndex_]),
                    static_cast<double>(structure_.zCoordList[atomIndex_])
                );
                frame.add_atom(atom, position);
                residue.add_atom(atomIndex_ - atomSkip_);
                atomIndex_++;
            }

            for (size_t l = 0; l < group.bondOrderList.size(); l++) {
                auto atom1 = static_cast<size_t>(group.bondAtomList[l * 2]);
                auto atom2 = static_cast<size_t>(group.bondAtomList[l * 2 + 1]);

                Bond::BondOrder bo;
                switch(group.bondOrderList[l]) {
                    case 1:
                        bo = Bond::SINGLE;
                        break;
                    case 2:
                        bo = Bond::DOUBLE;
                        break;
                    case 3:
                        bo = Bond::TRIPLE;
                        break;
                    case 4:
                        bo = Bond::QUADRUPLE;
                        break;
                    default:
                        bo = Bond::UNKNOWN;
                        break;
                }

                frame.add_bond(
                    atomOffset + atom1,
                    atomOffset + atom2,
                    bo
                );
            }

            if (groupIndex_ < structure_.secStructList.size()) {
                // We use the raw values here to directly match the MMTF spec
                // See https://github.com/rcsb/mmtf/blob/master/spec.md#secstructlist
                switch(structure_.secStructList[groupIndex_]) {
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
                    default:
                        break;
                }
            }

            // If the name of the current assembly is defined in the MMTF file.
            // Bioassemblies are optional, however.
            if (!current_assembly.empty()) {
                residue.set("assembly", current_assembly);
            }

            // This is a string in MMTF, differs from the name as then increments linearly
            // For example, the fourth chainid in ( A B A B) would be D, not B (the chainname)
            residue.set("chainid", structure_.chainIdList[chainIndex_]);

            // An integer
            residue.set("chainindex", chainIndex_);

            if (!structure_.chainNameList.empty()) {
                residue.set("chainname", structure_.chainNameList[chainIndex_]);
            }

            frame.add_residue(std::move(residue));
            groupIndex_++;
        }

        chainIndex_++;
    }

    modelIndex_++;
    for (size_t i = 0; i < structure_.bondAtomList.size() / 2; i++) {
        auto atom1 = static_cast<size_t>(structure_.bondAtomList[i * 2]);
        auto atom2 = static_cast<size_t>(structure_.bondAtomList[i * 2 + 1]);

        // We are below the atoms we care about
        if (atom1 < atomSkip_ || atom2 < atomSkip_) {
            continue;
        }

        // We are above the atoms we care about
        if (atom1 > atomIndex_ || atom2 > atomIndex_) {
            continue;
        }

        size_t atom_idx1 = atom1 - atomSkip_;
        size_t atom_idx2 = atom2 - atomSkip_;

        frame.add_bond(atom_idx1, atom_idx2);
    }

    atomSkip_ = atomIndex_;
}

void MMTFFormat::write(const Frame& frame) {
    // Used to add bonds  bonds
    auto writeAtomLoc = static_cast<size_t>(structure_.numAtoms);

    structure_.numModels++;
    structure_.chainsPerModel.emplace_back(0);

    std::string prev_chainId;
    std::string prev_chainName;
    const chemfiles::Residue* prev_residue = nullptr;

    auto& topology = frame.topology();
    auto& positions = frame.positions();
    for (size_t i = 0; i<frame.size(); i++) {
        structure_.numAtoms++;
        structure_.xCoordList.emplace_back(positions[i][0]);
        structure_.yCoordList.emplace_back(positions[i][1]);
        structure_.zCoordList.emplace_back(positions[i][2]);

        auto res_opt = topology.residue_for_atom(i);

        if (!res_opt) {
            continue;
        }

        // We've got a new resiude!
        if (prev_residue == &(*res_opt)) {
            auto& group = structure_.groupList.back();
            group.formalChargeList.emplace_back(frame[i].charge());
            group.atomNameList.emplace_back(frame[i].name());
            group.elementList.emplace_back(frame[i].type());

            continue;
        }

        const auto& current_chainId_opt = res_opt->get("chainid");
        const auto& current_chainName_opt = res_opt->get("chainname");
        std::string current_chainId;
        std::string current_chainName;

        if (current_chainId_opt && current_chainId_opt->kind() == Property::STRING) {
            current_chainId = current_chainId_opt->as_string();
        }

        if (current_chainName_opt && current_chainName_opt->kind() == Property::STRING) {
            current_chainName = current_chainName_opt->as_string();
        }

        if (current_chainName != prev_chainName ||
            current_chainId != prev_chainId) {

            structure_.numChains++;
            structure_.chainsPerModel.back()++;

            // No residues... yet
            structure_.groupsPerChain.emplace_back(0);

            structure_.chainIdList.emplace_back(current_chainId);
            structure_.chainNameList.emplace_back(current_chainName);

            prev_chainId = current_chainId;
            prev_chainName = current_chainName;
        }

        structure_.numGroups++;
        prev_residue = &(*res_opt);

        structure_.groupsPerChain.back() += 1;

        auto groupType = static_cast<int32_t>(structure_.groupList.size());
        structure_.groupTypeList.emplace_back(groupType);

        auto groupId = res_opt->id();
        structure_.groupIdList.emplace_back(groupId ? *groupId : 0);
        structure_.groupList.emplace_back();
        structure_.groupList.back().groupName = prev_residue->name();
        structure_.groupList.back().chemCompType =
            prev_residue->get("composition_type") &&
            prev_residue->get("composition_type")->kind() == Property::STRING ?
            prev_residue->get("composition_type")->as_string() :
            "other";

        auto& group = structure_.groupList.back();
        group.formalChargeList.emplace_back(frame[i].charge());
        group.atomNameList.emplace_back(frame[i].name());
        group.elementList.emplace_back(frame[i].type());
    }

    mmtf::BondAdder bondadd(structure_);
    auto& bonds = topology.bonds();
    auto& bond_orders = topology.bond_orders();
    for (size_t i = 0; i < bonds.size(); ++i) {

        int8_t bo;
        switch(bond_orders[i]) {
            case Bond::BondOrder::SINGLE:
                bo = 1;
                break;
            case Bond::BondOrder::DOUBLE:
                bo = 2;
                break;
            case Bond::BondOrder::TRIPLE:
                bo = 3;
                break;
            case Bond::BondOrder::QUADRUPLE:
                bo = 4;
                break;
            default:
                bo = 1;
                break;
        }

        auto atom1 = static_cast<int32_t>(bonds[i][0] + writeAtomLoc);
        auto atom2 = static_cast<int32_t>(bonds[i][1] + writeAtomLoc);
        bondadd(atom1, atom2, bo);
    }
}

MMTFFormat::~MMTFFormat() {
    if (!filename_.empty()) {
        mmtf::compressGroupList(structure_);
        encodeToFile(structure_, filename_);
    }
}
