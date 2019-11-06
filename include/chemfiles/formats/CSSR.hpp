// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_CSSR_HPP
#define CHEMFILES_FORMAT_CSSR_HPP

#include <cstdint>
#include <string>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

namespace chemfiles {
class Frame;

/// [CSSR] (Cambridge Structure Search and Retrieval) file format reader and
/// writer. Only one frame can be read or written to this format.
///
/// [CSSR]: http://www.chem.cmu.edu/courses/09-560/docs/msi/modenv/D_Files.html#944777
class CSSRFormat final: public TextFormat {
public:
    CSSRFormat(std::string path, File::Mode mode, File::Compression compression);

    void read_next(Frame& frame) override;
    void write_next(const Frame& frame) override;
    optional<uint64_t> forward() override;
};

template<> FormatInfo format_information<CSSRFormat>();

} // namespace chemfiles

#endif
