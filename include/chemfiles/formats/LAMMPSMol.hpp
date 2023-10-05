// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_LAMMPSMol_HPP
#define CHEMFILES_FORMAT_LAMMPSMol_HPP

#include <cstdint>
#include <string>
#include <memory>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

#include "chemfiles/external/optional.hpp"

namespace chemfiles {
class Frame;
class MemoryBuffer;
class FormatMetadata;

/// LAMMPSMol file format reader and writer.
///
class LAMMPSMolFormat final: public TextFormat {
public:
    LAMMPSMolFormat(std::string path, File::Mode mode, File::Compression compression):
        TextFormat(std::move(path), mode, compression), current_section_(HEADER){}

    LAMMPSMolFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode, File::Compression compression) :
        TextFormat(std::move(memory), mode, compression), current_section_(HEADER){}

    void read_next(Frame& frame) override;
    void write_next(const Frame& frame) override;
    optional<uint64_t> forward() override;

private:
    enum section_t {
        HEADER,
        COORDS,
        TYPES,
        MOLECULES,
        FRAGMENTS,
        CHARGES,
        DIAMETERS,
        MASSES,
        BONDS,
        ANGLES,
        DIHEDRALS,
        IMPROPERS,
        SPECIAL_BONDS_COUNTS,
        SPECIAL_BONDS,
        SHAKE_FLAGS,
        SHAKE_ATOMS,
        NOT_A_SECTION,
        IGNORED;
    } current_section_;


};

template<> const FormatMetadata& format_metadata<LAMMPSMolFormat>();

} // namespace chemfiles

#endif
