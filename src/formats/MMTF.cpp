// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/MMTF.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<MMTFFormat>() {
    return FormatInfo("MMTF").with_extension(".mmtf").description(
        "MMTF (RCSB Protein Data Bank) binary format"
    );
}

MMTFFormat::MMTFFormat(const std::string& path, File::Mode mode)
  : file_(path, mode),
    modelIndex_(0), chainIndex_(0), groupIndex_(0), atomIndex_(0),
    atomSkip_(0) {
}

size_t MMTFFormat::nsteps() {
    return static_cast<size_t>(file_->numModels);
}

void MMTFFormat::read_step(const size_t step, Frame& frame) {

    modelIndex_ = 0;
    chainIndex_ = 0;
    groupIndex_ = 0;
    atomIndex_  = 0;
    atomSkip_   = 0;

    // Fast-forward, keeping all indecies updated
    while(modelIndex_ != step) {
        size_t chainsPerModel = static_cast<size_t>(file_->chainsPerModel[modelIndex_]);
        while(chainIndex_ != chainsPerModel) {
            size_t groupsPerChain = static_cast<size_t>(file_->groupsPerChain[chainIndex_]);
            while(groupIndex_ != groupsPerChain) {
                auto group = file_->groupList[file_->groupTypeList[groupIndex_]];
                size_t atomCount = static_cast<size_t>(group.atomNameListCount);
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

    UnitCell cell (file_->unitCell[0], file_->unitCell[1], file_->unitCell[2],
                   file_->unitCell[3], file_->unitCell[4], file_->unitCell[5]);

    frame.set_cell(cell);

    size_t modelChainCount = static_cast<size_t>(file_->chainsPerModel[modelIndex_]);

    for (size_t j = 0; j < modelChainCount; j++) {

        size_t chainGroupCount = static_cast<size_t>(file_->groupsPerChain[chainIndex_]);

        // A group is like a residue or other molecule in a PDB file.
        for (size_t k = 0; k < chainGroupCount; k++) {
            MMTF_GroupType group = file_->groupList[file_->groupTypeList[groupIndex_]];

            size_t groupId = static_cast<size_t>(file_->groupIdList[groupIndex_]);
            Residue res(group.groupName, groupId);
            // TODO: Use group.chemCompType to assign linkage

            // Save the offset before we go changing it
            size_t atomOffset = atomIndex_ - atomSkip_;

            size_t groupSize = static_cast<size_t>(group.atomNameListCount);
            for (size_t l = 0; l < groupSize; l++) {
                auto atom = Atom(group.atomNameList[l]);
                atom.set_type(group.elementList[l]);
                frame.add_atom(atom, Vector3D(file_->xCoordList[atomIndex_],
                                              file_->yCoordList[atomIndex_],
                                              file_->zCoordList[atomIndex_]));
                res.add_atom(atomIndex_);
                atomIndex_++;
            }

            for (size_t l = 0; l < group.bondOrderListCount; l++) {

                size_t atom1 = static_cast<size_t>(group.bondAtomList[l * 2]);
                size_t atom2 = static_cast<size_t>(group.bondAtomList[l * 2 + 1]);
                frame.add_bond(atomOffset + atom1,
                               atomOffset + atom2);

                // TODO add bond information using group.bondOrderList[l]
            }

            frame.add_residue(res);
            groupIndex_++;
        }

        chainIndex_++;
    }

    modelIndex_++;
}

MMTFFormat::~MMTFFormat() noexcept {
}
