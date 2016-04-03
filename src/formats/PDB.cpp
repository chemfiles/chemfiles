/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <sstream>
#include <cassert>
#include <algorithm>
#include <cctype>

#include "chemfiles/formats/PDB.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/Logger.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/File.hpp"

using namespace chemfiles;

std::string PDBFormat::description() const {
    return "PDB file format.";
}

// Fast-forward the file for `nsteps`. If the files has less than `nsteps`, the
// file cursor will be at EOF.
static void forward(TextFile& file, size_t nsteps);

// PDB record handled by chemfiles. Any record not in this enum are not yet
// implemented.
enum class Record {
    // Records containing usefull data
    CRYST1,
    ATOM,
    HETATM,
    CONECT,
    // End of file
    END,
    // Ignored records
    _IGNORED_,
    // Unknown record type
    _UNKNOWN_,
};

// Get the record type for a line.
static Record get_record(const std::string&);

static inline std::string trim(const std::string &s) {
   auto front = std::find_if_not(s.begin(), s.end(), [](int c){return std::isspace(c);} );
   auto back = std::find_if_not(s.rbegin(), s.rend(), [](int c){return std::isspace(c);} ).base();
   return (back <= front ? std::string() : std::string(front, back));
}

PDBFormat::PDBFormat(File& f) : Format(f), textfile_(dynamic_cast<TextFile&>(file_)) {}

size_t PDBFormat::nsteps() const {
    textfile_.rewind();
    size_t n = 0;
    while (true) {
        forward(textfile_, 1);
        if (textfile_.eof()) {
            break;
        } else {
            n++;
        }
    }
    textfile_.rewind();
    return n;
}

void PDBFormat::read_step(const size_t step, Frame& frame){
    textfile_.rewind();
    forward(textfile_, step);
    read(frame);
}

void PDBFormat::read(Frame& frame){
    assert(frame.natoms() == 0);
    std::string line;
    while (!textfile_.eof()) {
        line = textfile_.getline();
        auto record = get_record(line);
        switch (record) {
            case Record::CRYST1:
                read_cryst1(frame, line);
                break;
            case Record::ATOM:
            case Record::HETATM:
                read_atom(frame, line);
                break;
            case Record::CONECT:
                read_conect(frame, line);
                break;
            case Record::END:
                return; // We have read a frame!
            case Record::_IGNORED_:
                break; // Nothing to do
            case Record::_UNKNOWN_:
                Logger::warn("Unknown PDB record: " + line);
                break;
        }
    }
}

void PDBFormat::read_cryst1(Frame& frame, const std::string& line) {
    assert(line.substr(0, 6) == "CRYST1");
    if (line.length() < 54) {
        throw FormatError("CRYST1 record is too small: '" + line + "'");
    }
    try {
        auto a = std::stof(line.substr(6, 9));
        auto b = std::stof(line.substr(15, 9));
        auto c = std::stof(line.substr(24, 9));
        auto alpha = std::stof(line.substr(33, 7));
        auto beta = std::stof(line.substr(40, 7));
        auto gamma = std::stof(line.substr(47, 7));
        auto cell = UnitCell(a, b, c, alpha, beta, gamma);

        frame.set_cell(cell);

        auto space_group = trim(line.substr(55, 10));
        if (space_group != "P 1" && space_group != "P1") {
            Logger::warn(
                "Space group is not P1 (got '" + space_group + "') in '" +
                file_.filename() + "', ignored."
            );
        }
    } catch (std::invalid_argument& e) {
        throw FormatError("Could not read CRYST1 record: '" + line + "'");
    }
}

void PDBFormat::read_atom(Frame& frame, const std::string& line) {
    assert(line.substr(0, 6) == "ATOM  " || line.substr(0, 6) == "HETATM");

    if (line.length() < 54) {
        throw FormatError(line.substr(0, 6) + " record is too small: '" + line + "'");
    }

    auto i = frame.natoms();
    frame.resize(i + 1);

    auto name = trim(line.substr(12, 4));
    auto atom = Atom(name);
    auto& position = frame.positions()[i];
    try {
        position[0] = std::stof(line.substr(31, 8));
        position[1] = std::stof(line.substr(38, 8));
        position[2] = std::stof(line.substr(46, 8));
    } catch (std::invalid_argument& e) {
        throw FormatError("Could not read positions in record: '" + line + "'");
    }

    frame.topology()[i] = atom;
}

void PDBFormat::read_conect(Frame& frame, const std::string& full_line) {
    auto line = trim(full_line);
    assert(line.substr(0, 6) == "CONECT");

    // Helper lambdas
    auto add_bond = [&frame, &line](size_t i, size_t j) {
        if (i >= frame.natoms() || j >= frame.natoms()) {
            Logger::warn("Bad atomic numbers in CONECT, ignored. (" + line + ")");
            return;
        }
        frame.topology().add_bond(i, j);
    };

    auto read_index = [&line] (size_t initial) -> size_t {
        try {
            // PDB indexing is 1-based, and chemfiles is 0-based
            return std::stoul(line.substr(initial, 5)) - 1;
        } catch (std::invalid_argument& e) {
            throw FormatError("Could not read atomic number in: '" + line + "'");
        }
    };

    auto i = read_index(6);

    if (line.length() > 11) {
        auto j = read_index(11);
        add_bond(i, j);
    } else { return; }

    if (line.length() > 16) {
        auto j = read_index(16);
        add_bond(i, j);
    } else { return; }

    if (line.length() > 21) {
        auto j = read_index(21);
        add_bond(i, j);
    } else { return; }

    if (line.length() > 26) {
        auto j = read_index(26);
        add_bond(i, j);
    } else { return; }
}

void forward(TextFile& file, size_t nsteps) {
    size_t i=0;
    // Move the file pointer to the good position step by step, as the number of
    // atoms may not be constant
    std::string line;
    while (i < nsteps && !file.eof()) {
        line = file.getline();
        if (line.substr(0, 3) == "END") {
            i++;
        }
    }
}

Record get_record(const std::string& line) {
    auto rec = line.substr(0, 6);
    if (rec.substr(0, 3) == "END") { // Handle missing whitespaces in END
        return Record::END;
    } else if (rec == "CRYST1") {
        return Record::CRYST1;
    } else if (rec == "ATOM  ") {
        return Record::ATOM;
    } else if (rec == "HETATM") {
        return Record::HETATM;
    } else if (rec == "CONECT") {
        return Record::CONECT;
    } else if (rec == "REMARK" || rec == "MASTER" || rec == "AUTHOR" ||
               rec == "CAVEAT" || rec == "COMPND" || rec == "EXPDTA" ||
               rec == "KEYWDS" || rec == "OBSLTE" || rec == "SOURCE" ||
               rec == "SPLIT " || rec == "SPRSDE" || rec == "TITLE " ||
               rec == "JRNL  ") {
        return Record::_IGNORED_;
    } else {
        return Record::_UNKNOWN_;
    }
}
