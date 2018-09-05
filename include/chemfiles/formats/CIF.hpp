// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_CIF_HPP
#define CHEMFILES_FORMAT_CIF_HPP

#include <map>
#include <unordered_map>

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/UnitCell.hpp"

namespace chemfiles {

/// [CIF] (Crystallographic Information Framework) format reader and writer.
///
/// [CIF]: https://www.iucr.org/resources/cif
class CIFFormat final: public Format {
public:
    CIFFormat(std::string path, File::Mode mode, File::Compression compression);
    ~CIFFormat() noexcept;

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;
private:
    std::unique_ptr<TextFile> file_;
    /// Map of STAR records to their index
    std::unordered_map<std::string, size_t> atom_site_map_;
    /// Map of residues, indexed by residue id and chainid.
    std::map<std::pair<std::string, size_t>, Residue> residues_;
    /// Set to true if the file is based on fractional coordinates.
    /// Set to false if the file is based on cartn coordinates.
    bool uses_fract_;
    /// Storing the positions of all the steps in the file, so that we can
    /// just `seekg` them instead of reading the whole step.
    std::vector<std::streampos> steps_positions_;
    /// The cell for all frames
    UnitCell cell_;
    /// Number of models written/read to the file.
    size_t models_;
    /// Did we wrote a frame to the file? This is used to check wheter we need
    /// to write a final `END` record in the destructor
    bool written_ = false;
};

template<> FormatInfo format_information<CIFFormat>();

} // namespace chemfiles

#endif
