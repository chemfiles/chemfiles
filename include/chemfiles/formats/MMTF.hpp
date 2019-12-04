// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_MMTF_HPP
#define CHEMFILES_FORMAT_MMTF_HPP

#include <string>

#include <mmtf/structure_data.hpp>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Residue.hpp"

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
    MMTFFormat(const MMTFFormat&) = delete;
    MMTFFormat& operator=(const MMTFFormat&) = delete;
    MMTFFormat(MMTFFormat&&) = default;
    MMTFFormat& operator=(MMTFFormat&&) = default;

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;

private:
    /// add a single residue to the structure_, using the data from the frame
    void add_residue_to_structure(const Frame& frame, const Residue& residue);

    /// A function to translate from the index in MMTF lists to an atom id
    /// suitable for chemfiles: starts at 0 for each model, and correspond to
    /// the initial atom index if it exists. This is used when reading.
    size_t atom_id(size_t mmtf_id);

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

    // Since MMTF uses model->chain->residue->atom as storage model, and
    // chemfiles do not enforce that residues contains contiguous atoms, the
    // atoms can be re-ordered when adding them to a MMTF structure. This vector
    // stores the correspondance chemfiles index => MMTF index to be able to add
    // the right bonds.
    //
    // This is only used when writing
    std::vector<int32_t> new_atom_indexes_;
};

template<> FormatInfo format_information<MMTFFormat>();

} // namespace chemfiles

#endif
