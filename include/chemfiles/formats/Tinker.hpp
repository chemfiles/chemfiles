// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_TINKER_HPP
#define CHEMFILES_FORMAT_TINKER_HPP

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"

namespace chemfiles {

/// Tinker XYZ file format.
///
/// The format is described here: http://chembytes.wikidot.com/tnk-tut00#toc2.
/// Additionally to the original Tinker XYZ format, chemfiles also supports the
/// files with unit cell data, as introduced in Tinker 6.3.
///
/// This format is associated with the .arc extension, but not the .xyz
/// extension, which is used for the standard XYZ format.
class TinkerFormat final: public Format {
public:
    TinkerFormat(const std::string& path, File::Mode mode);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;
private:
    /// Text file where we read from
    std::unique_ptr<TextFile> file_;
    /// Storing the positions of all the steps in the file, so that we can
    /// just `seekg` them instead of reading the whole step.
    std::vector<std::streampos> steps_positions_;
};

template<> FormatInfo format_information<TinkerFormat>();

} // namespace chemfiles

#endif
