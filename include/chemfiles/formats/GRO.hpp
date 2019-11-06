// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_GRO_HPP
#define CHEMFILES_FORMAT_GRO_HPP

#include <cstdint>
#include <map>
#include <string>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Residue.hpp"

namespace chemfiles {
class Frame;

/// [GRO] file format reader and writer.
///
/// [GRO]: http://manual.gromacs.org/current/online/gro.html
class GROFormat final: public TextFormat {
public:
    GROFormat(std::string path, File::Mode mode, File::Compression compression):
        TextFormat(std::move(path), mode, compression) {}

    void read_next(Frame& frame) override;
    void write_next(const Frame& frame) override;
    optional<uint64_t> forward() override;

private:
    /// Map of residues, indexed by residue id.
    std::map<size_t, Residue> residues_;
};

template<> FormatInfo format_information<GROFormat>();

} // namespace chemfiles

#endif
