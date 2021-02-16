// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_LAMMPS_ATOM_HPP
#define CHEMFILES_FORMAT_LAMMPS_ATOM_HPP

#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

#include "chemfiles/external/optional.hpp"

namespace chemfiles {
class Frame;
class MemoryBuffer;
class FormatMetadata;

/// LAMMPS Atom file format reader and writer.
class LAMMPSAtomFormat final : public TextFormat {
  public:
    LAMMPSAtomFormat(std::string path, File::Mode mode, File::Compression compression)
        : TextFormat(std::move(path), mode, compression) {}

    LAMMPSAtomFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode,
                     File::Compression compression)
        : TextFormat(std::move(memory), mode, compression) {}

    void read_next(Frame& frame) override;
    optional<uint64_t> forward() override;

  private:
    std::array<double, 3> read_cell(Frame& frame);
};

template <> const FormatMetadata& format_metadata<LAMMPSAtomFormat>();

} // namespace chemfiles

#endif
