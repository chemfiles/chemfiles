// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_MOL2_HPP
#define CHEMFILES_FORMAT_MOL2_HPP

#include <unordered_map>

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Residue.hpp"

namespace chemfiles {

/// [MOL2][MOL2] file format reader and writer.
///
/// For multi-frame trajectories, we follow the convention of VMD to use multiple
/// `END` records, separating the steps.
///
/// [MOL2]: 
class MOL2Format final: public Format {
public:
    MOL2Format(const std::string& path, File::Mode mode);

    MOL2Format(const MOL2Format&) = delete;
    MOL2Format& operator=(const MOL2Format&) = delete;
    MOL2Format(MOL2Format&&) = default;
    MOL2Format& operator=(MOL2Format&&) = default;

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;

private:
    // Read Atoms
    void read_atoms(Frame& frame, size_t natoms, bool charges);

    // Read Bonds
    void read_bonds(Frame& frame, size_t nbonds);

    std::unique_ptr<TextFile> file_;
    /// Map of residues, indexed by residue id.
    std::unordered_map<size_t, Residue> residues_;
    /// Storing the positions of all the steps in the file, so that we can
    /// just `seekg` them instead of reading the whole step.
    std::vector<std::streampos> steps_positions_;
};

template<> FormatInfo format_information<MOL2Format>();

} // namespace chemfiles

#endif
