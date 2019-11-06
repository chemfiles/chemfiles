// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_MMCIF_HPP
#define CHEMFILES_FORMAT_MMCIF_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/UnitCell.hpp"

namespace chemfiles {
class Frame;

/// [mmCIF] (Crystallographic Information Framework) for MacroMolecules
///
/// [mmCIF]: http://mmcif.wwpdb.org/
class mmCIFFormat final: public Format {
public:
    mmCIFFormat(std::string path, File::Mode mode, File::Compression compression);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;
private:
    TextFile file_;
    /// Map of STAR records to their index
    std::map<std::string, size_t> atom_site_map_;
    /// Map of residues, indexed by residue id and chainid.
    std::map< std::pair<std::string, size_t>, Residue> residues_;
    /// Set to true if the file is based on fractional coordinates.
    /// Set to false if the file is based on cartn coordinates.
    bool uses_fract_;
    /// Storing the positions of all the steps in the file, so that we can
    /// just `seekpos` them instead of reading the whole step.
    std::vector<uint64_t> steps_positions_;
    /// The cell for all frames
    UnitCell cell_;
    /// Number of models written to the file.
    size_t models_;
    /// Number of atoms written to the file.
    size_t atoms_;
    /// Frame properties need to be stored
    std::string name_;
    std::string pdb_idcode_;
};

template<> FormatInfo format_information<mmCIFFormat>();

} // namespace chemfiles

#endif
