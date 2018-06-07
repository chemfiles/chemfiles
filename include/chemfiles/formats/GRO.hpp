// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_GRO_HPP
#define CHEMFILES_FORMAT_GRO_HPP

#include <map>

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Residue.hpp"

namespace chemfiles {

/// [GRO] file format reader and writer.
///
/// [GRO]: http://manual.gromacs.org/current/online/gro.html
class GROFormat final: public Format {
public:
    GROFormat(const std::string& path, File::Mode mode);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;
private:
    /// Map of residues, indexed by residue id.
    std::map<size_t, Residue> residues_;
    /// Text file where we read from
    std::unique_ptr<TextFile> file_;
    /// Storing the positions of all the steps in the file, so that we can
    /// just `seekg` them instead of reading the whole step.
    std::vector<std::streampos> steps_positions_;
};

template<> FormatInfo format_information<GROFormat>();

} // namespace chemfiles

#endif
