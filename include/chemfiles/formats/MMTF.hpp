// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_MMTF_HPP
#define CHEMFILES_FORMAT_MMTF_HPP

#include <string>

#include <mmtf/structure_data.hpp>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

namespace chemfiles {
class Frame;

/// [MMTF][MMTF] file format reader and writer.
///
/// For multi-frame trajectories, we follow the PyMOL convention to use multiple
/// models for different frames
///
/// [MMTF]: https://mmtf.rcsb.org/
class MMTFFormat final: public Format {
public:
    MMTFFormat(std::string path, File::Mode mode, File::Compression compression);

    ~MMTFFormat() override;

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;

private:

    /// MMTF-CPP object holding all MacroMolecular information.
    /// This can be read and modified.
    mmtf::StructureData structure_;

    /// Location of MMTF file on disk. Only used if opened in write mode.
    std::string filename_;

    /// Current model being read. Ranges from [0, structure.numModels)
    size_t modelIndex_ = 0;

    /// Current chain being read. Ranges from [0, structure.numChains)
    size_t chainIndex_ = 0;

    /// Current group (residue) being read. Ranges from [0, structure.numGroups)
    size_t groupIndex_ = 0;

    /// Current atom being read. Ranges from [0, structure.numAtoms)
    size_t atomIndex_ = 0;

    /// Number of atoms before the current model.
    size_t atomSkip_ = 0;
};

template<> FormatInfo format_information<MMTFFormat>();

} // namespace chemfiles

#endif
