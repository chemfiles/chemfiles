// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <mmtf.hpp>

#include "chemfiles/formats/MMTF.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/Frame.hpp"

#include "chemfiles/files/GzFile.hpp"
#include "chemfiles/files/XzFile.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<MMTFFormat>() {
    return FormatInfo("MMTF").with_extension(".mmtf").description(
        "MMTF (RCSB Protein Data Bank) binary format"
    );
}

MMTFFormat::MMTFFormat(std::string path, File::Mode mode, File::Compression compression) {
    if (mode == File::READ) {
        if (compression == File::GZIP) {
            gzstreambuf gz_buff;
            gz_buff.open(path, "rb");
            std::stringstream buffer;
            buffer << &gz_buff;
            mmtf::decodeFromBuffer(structure_, buffer.str().data(), buffer.str().size());
        } else if (compression == File::LZMA) {
            xzstreambuf xz_buff;
            xz_buff.open(path, "rb");
            std::stringstream buffer;
            buffer << &xz_buff;
            mmtf::decodeFromBuffer(structure_, buffer.str().data(), buffer.str().size());
        } else { // Just in case the user specified MMTF without a proper extension
            mmtf::decodeFromFile(structure_, path);
        }
        if (!structure_.hasConsistentData()) {
            throw format_error("Issue with: {}. Please ensure it is valid MMTF", path);
        }
    } else if (mode == File::WRITE || mode == File::APPEND) {
        throw file_error("Writting an MMTF File is not currently implemented");
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
        frame.set_cell(UnitCell(
            structure_.unitCell[0], structure_.unitCell[1], structure_.unitCell[2],
            structure_.unitCell[3], structure_.unitCell[4], structure_.unitCell[5]
        ));
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

            // Save the offset before we go changing it
            size_t atomOffset = atomIndex_ - atomSkip_;

            auto groupSize = group.atomNameList.size();
            for (size_t l = 0; l < groupSize; l++) {
                auto atom = Atom(group.atomNameList[l]);
                atom.set_type(group.elementList[l]);
                atom.set_charge(group.formalChargeList[l]);
                auto position = Vector3D(
                    structure_.xCoordList[atomIndex_],
                    structure_.yCoordList[atomIndex_],
                    structure_.zCoordList[atomIndex_]
                );
                atom.set("is_hetatm", mmtf::is_hetatm(group.chemCompType.c_str()));
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
