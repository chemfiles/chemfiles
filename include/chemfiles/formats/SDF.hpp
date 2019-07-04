// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_SDF_HPP
#define CHEMFILES_FORMAT_SDF_HPP

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"

namespace chemfiles {
class Frame;

/// [SDF] file format reader and writer.
///
/// [SDF]: http://accelrys.com/products/collaborative-science/biovia-draw/ctfile-no-fee.html
class SDFFormat final: public Format {
public:
    SDFFormat(std::string path, File::Mode mode, File::Compression compression);

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

template<> FormatInfo format_information<SDFFormat>();

} // namespace chemfiles

#endif
