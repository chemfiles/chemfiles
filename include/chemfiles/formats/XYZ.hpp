// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_XYZ_HPP
#define CHEMFILES_FORMAT_XYZ_HPP

#include <iosfwd>
#include <string>

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"

namespace chemfiles {
class Frame;

/// [XYZ] file format reader and writer.
///
/// [XYZ]: http://openbabel.org/wiki/XYZ
class XYZFormat final: public TextFormat {
public:
    XYZFormat(std::string path, File::Mode mode, File::Compression compression):
        TextFormat(std::move(path), mode, compression){}

    void read_next(Frame& frame) override;
    void write_next(const Frame& frame) override;
    std::streampos forward() override;
};

template<> FormatInfo format_information<XYZFormat>();

} // namespace chemfiles

#endif
