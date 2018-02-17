// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_MMTF_HPP
#define CHEMFILES_FORMAT_MMTF_HPP

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/files/MMTFFile.hpp"

namespace chemfiles {

/// [MMTF][MMTF] file format reader and writer.
///
/// For multi-frame trajectories, we follow the PyMOL convention to use multiple
/// models for different frames
///
/// [MMTF]: https://mmtf.rcsb.org/
class MMTFFormat final: public Format {
public:
    MMTFFormat(const std::string& path, File::Mode mode);

    ~MMTFFormat() noexcept override;
    MMTFFormat(const MMTFFormat&) = delete;
    MMTFFormat& operator=(const MMTFFormat&) = delete;
    MMTFFormat(MMTFFormat&&) = default;
    MMTFFormat& operator=(MMTFFormat&&) = default;

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    size_t nsteps() override;

private:

    MMTFFile file_;
    size_t modelIndex_;
    size_t chainIndex_;
    size_t groupIndex_;
    size_t atomIndex_;
    size_t atomSkip_;
};

template<> FormatInfo format_information<MMTFFormat>();

} // namespace chemfiles

#endif
