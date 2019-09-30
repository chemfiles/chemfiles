// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_XTC_FORMAT_HPP
#define CHEMFILES_XTC_FORMAT_HPP

#include <string>
#include <vector>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/files/XDRFile.hpp"

namespace chemfiles {
class Frame;

class XTCFormat final : public Format {
  public:
    XTCFormat(std::string path, File::Mode mode, File::Compression compression);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;

  private:
    /// Associated XDR file
    XDRFile file_;
    /// The next step to read
    size_t step_ = 0;
};

template <> FormatInfo format_information<XTCFormat>();

} // namespace chemfiles

#endif
