// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_CIF_HPP
#define CHEMFILES_FORMAT_CIF_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "gemmi/cif.hpp"
#include "gemmi/smcif.hpp"
#include "gemmi/to_cif.hpp"

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/UnitCell.hpp"

namespace chemfiles {
class Frame;
class MemoryBuffer;

/// [CIF] (Crystallographic Information Framework)
///
/// [CIF]: https://en.wikipedia.org/wiki/Crystallographic_Information_File
class CIFFormat final: public Format {
public:
    CIFFormat(std::string path, File::Mode mode, File::Compression compression) :
        file_(std::move(path), mode, compression), models_(0) {
        init_();
    }

    CIFFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode, File::Compression compression) :
        file_(std::move(memory), mode, compression), models_(0) {
        init_();
    }

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;
private:
    /// Initialize important variables
    void init_();
    /// Underlying file representation
    TextFile file_;
    /// Store all structures, reading the whole file during init_()
    std::vector<gemmi::SmallStructure> structures_;
    /// Number of models written to the file.
    size_t models_;
};

template<> FormatInfo format_information<CIFFormat>();

} // namespace chemfiles

#endif
