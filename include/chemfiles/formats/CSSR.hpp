// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_CSSR_HPP
#define CHEMFILES_FORMAT_CSSR_HPP

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"

namespace chemfiles {

/// [CSSR] (Cambridge Structure Search and Retrieval) file format reader and
/// writer. Only one frame can be read or written to this format.
///
/// [CSSR]: http://www.chem.cmu.edu/courses/09-560/docs/msi/modenv/D_Files.html#944777
class CSSRFormat final: public Format {
public:
    CSSRFormat(const std::string& path, File::Mode mode);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;
private:
    /// Text file where we read from
    std::unique_ptr<TextFile> file_;
    /// Did we already wrote a frame to this file
    bool written_ = false;
};

template<> FormatInfo format_information<CSSRFormat>();

} // namespace chemfiles

#endif
