// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_PDB_HPP
#define CHEMFILES_FORMAT_PDB_HPP

#include <map>

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
    PDBFormat(const std::string& path, File::Mode mode);

    ~PDBFormat() noexcept override;
    PDBFormat(const PDBFormat&) = delete;
    PDBFormat& operator=(const PDBFormat&) = delete;
    PDBFormat(PDBFormat&&) = default;
    PDBFormat& operator=(PDBFormat&&) = default;

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;

private:
    // Read CRYST1 record
    void read_CRYST1(Frame& frame, const std::string& line);
    // Read ATOM and HETATM records
    void read_ATOM(Frame& frame, const std::string& line, bool is_hetatm);

    // Read CONECT record
    void read_CONECT(Frame& frame, const std::string& line);

    std::unique_ptr<TextFile> file_;
    /// Map of residues, indexed by residue id.
    std::map<size_t, Residue> residues_;
    /// Storing the positions of all the steps in the file, so that we can
    /// just `seekg` them instead of reading the whole step.
    std::vector<std::streampos> steps_positions_;
    /// Number of models written/read to the file.
    size_t models_;
    /// Did we wrote a frame to the file? This is used to check wheter we need
    /// to write a final `END` record in the destructor
    bool written_ = false;
};

template<> FormatInfo format_information<PDBFormat>();

} // namespace chemfiles

#endif
