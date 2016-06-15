/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_FORMAT_PDB_HPP
#define CHEMFILES_FORMAT_PDB_HPP

#include <string>

#include "chemfiles/Format.hpp"
#include "chemfiles/TrajectoryFactory.hpp"

namespace chemfiles {

/*!
 * @class PDBFormat formats/PDB.hpp formats/PDB.cpp
 * @brief PDB file format reader and writer.
 *
 * The format is described at
 * ftp://ftp.wwpdb.org/pub/pdb/doc/format_descriptions/Format_v33_A4.pdf
 *
 * For multi-frame trajectories, we follow the convention of VMD to use multiple
 * `END` records, separating the steps.
 */
class PDBFormat final: public Format {
public:
    PDBFormat(File& f);
    ~PDBFormat() = default;

    virtual void read_step(size_t step, Frame& frame) override;
    virtual void read(Frame& frame) override;
    virtual void write(const Frame& frame) override;
    virtual std::string description() const override;
    virtual size_t nsteps() override;

    // Register the PDB format with the ".pdb" extension and the "PDB" description.
    FORMAT_NAME(PDB)
    FORMAT_EXTENSION(.pdb)
private:
    // Read CRYST1 record
    void read_cryst1(Frame& frame, const std::string& line);
    // Read ATOM an HETATM records
    void read_atom(Frame& frame, const std::string& line);
    // Read CONECT record
    void read_conect(Frame& frame, const std::string& line);

    TextFile& textfile_;
};

} // namespace chemfiles

#endif
