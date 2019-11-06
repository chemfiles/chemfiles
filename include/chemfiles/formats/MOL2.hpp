// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_MOL2_HPP
#define CHEMFILES_FORMAT_MOL2_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Residue.hpp"  // IWYU pragma: keep

namespace chemfiles {
class Frame;

/// [MOL2][MOL2] file format reader and writer.
///
/// For multi-frame trajectories, we follow the convention of VMD to use multiple
/// `END` records, separating the steps.
///
/// [MOL2]:
class MOL2Format final: public TextFormat {
public:
    MOL2Format(std::string path, File::Mode mode, File::Compression compression):
        TextFormat(std::move(path), mode, compression) {}

    void read_next(Frame& frame) override;
    void write_next(const Frame& frame) override;
    optional<uint64_t> forward() override;

private:
    // Read Atoms
    void read_atoms(Frame& frame, size_t natoms, bool charges);

    // Read Bonds
    void read_bonds(Frame& frame, size_t nbonds);

    /// Map of residues, indexed by residue id.
    std::unordered_map<size_t, Residue> residues_;
};

template<> FormatInfo format_information<MOL2Format>();

} // namespace chemfiles

#endif
