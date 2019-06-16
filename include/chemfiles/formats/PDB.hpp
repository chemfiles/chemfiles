// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_PDB_HPP
#define CHEMFILES_FORMAT_PDB_HPP

#include <map>
#include <vector>
#include <tuple>

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Residue.hpp"

namespace chemfiles {

/// [PDB][PDB] file format reader and writer.
///
/// For multi-frame trajectories, we follow the convention of VMD to use multiple
/// `END` records, separating the steps.
///
/// [PDB]: ftp://ftp.wwpdb.org/pub/pdb/doc/format_descriptions/Format_v33_A4.pdf
class PDBFormat final: public Format {
public:
    PDBFormat(std::string path, File::Mode mode, File::Compression compression);

    ~PDBFormat() override;

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;

    // Connect residues based on a predefined table
    static void link_standard_residue_bonds(Frame& frame);

private:
    // Read CRYST1 record
    void read_CRYST1(Frame& frame, const std::string& line);
    // Read ATOM and HETATM records
    void read_ATOM(Frame& frame, const std::string& line, bool is_hetatm);
    // Read secondary structure records. All push secinfo_ vector if line is valid
    void read_HELIX(const std::string& line);
    // reads SHEET and TURN records. i1 and i2 are the indicies of the chain ids
    void read_secondary(const std::string& line, size_t i1, size_t i2,
                        const std::string& sec);
    // Read CONECT record
    void read_CONECT(Frame& frame, const std::string& line);
    // Runs when a chain is terminated to update residue information
    void chain_ended(Frame& frame);

    std::unique_ptr<TextFile> file_;

    /// This typedef represent the 'full' name of a residue, this means the
    /// chainid, the residue sequence id, and the residue insertion code
    using residue_info = std::tuple<char, size_t, char>;

    /// Map where the key is the residue_info and the value is a residue object
    std::map<residue_info, Residue> residues_;
    /// Storing the positions of all the steps in the file, so that we can
    /// just `seekg` them instead of reading the whole step.
    std::vector<std::streampos> steps_positions_;
    /// Number of models written/read to the file.
    size_t models_;
    /// List of all atom offsets. This maybe pushed in read_ATOM or if a TER
    /// record is found. It is reset every time a frame is read.
    std::vector<size_t> atom_offsets_;
    /// Did we wrote a frame to the file? This is used to check wheter we need
    /// to write a final `END` record in the destructor
    bool written_ = false;
    /// Store secondary structure information. Each item is a tuple, containing
    /// the starting and ending residue_info of the seconary structure and a
    /// string which is an written description of the secondary structure
    std::vector<std::tuple<residue_info, residue_info, std::string>> secinfo_;
};

template<> FormatInfo format_information<PDBFormat>();

} // namespace chemfiles

#endif
