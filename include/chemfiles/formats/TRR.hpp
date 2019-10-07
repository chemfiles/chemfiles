// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_TRR_FORMAT_HPP
#define CHEMFILES_TRR_FORMAT_HPP

#include "chemfiles/Format.hpp"
#include "chemfiles/files/TRRFile.hpp"

namespace chemfiles {

class TRRFormat final : public Format {
  public:
    TRRFormat(std::string path, File::Mode mode, File::Compression compression);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;

  private:
    void set_positions(const std::vector<float>& x, Frame& frame);
    void get_positions(std::vector<float>& x, const Frame& frame);
    void set_velocities(const std::vector<float>& v, Frame& frame);
    void get_velocities(std::vector<float>& v, const Frame& frame);
    void set_cell(matrix box, Frame& frame);
    void get_cell(matrix box, const Frame& frame);

    /// Reference to the associated file
    TRRFile trr_;
    /// The next step to read
    size_t step_ = 0;
};

template <> FormatInfo format_information<TRRFormat>();

} // namespace chemfiles

#endif
