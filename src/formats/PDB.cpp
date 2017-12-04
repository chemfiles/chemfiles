// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/PDB.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<PDBFormat>() {
    return FormatInfo("PDB").with_extension(".pdb").description(
        "PDB (RCSB Protein Data Bank) text format"
    );
}

/// Check the number of digits before the decimal separator to be sure than
/// we can represen them. In case of error, use the given `context` in the error
/// message
static void check_values_size(const Vector3D& values, unsigned width, const std::string& context);
/// Fast-forward the file for one step, returning `false` if the file does
/// not contain one more step.
static bool forward(TextFile& file);


// PDB record handled by chemfiles. Any record not in this enum are not yet
// implemented.
enum class Record {
    // Records containing usefull data
    CRYST1,
    ATOM,
    HETATM,
    CONECT,
    // Beggining of model.
    MODEL,
    // End of model.
    ENDMDL,
    // End of file
    END,
    // Ignored records
    IGNORED_,
    // Unknown record type
    UNKNOWN_,
};

// Get the record type for a line.
static Record get_record(const std::string& line);

PDBFormat::PDBFormat(const std::string& path, File::Mode mode)
  : file_(TextFile::create(path, mode)), models_(0) {
    while (!file_->eof()) {
        auto position = file_->tellg();
        if (!file_ || position == std::streampos(-1)) {
            throw format_error("IO error while reading '{}' as PDB", path);
        }
        if (forward(*file_)) {
            steps_positions_.push_back(position);
        }
    }
    file_->rewind();
}

size_t PDBFormat::nsteps() {
    return steps_positions_.size();
}

void PDBFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

void PDBFormat::read(Frame& frame) {
    frame.resize(0);
    residues_.clear();

    while (!file_->eof()) {
        auto line = file_->readline();
        auto position = file_->tellg();
        auto record = get_record(line);
        switch (record) {
        case Record::CRYST1:
            read_CRYST1(frame, line);
            break;
        case Record::ATOM:
            read_ATOM(frame, line, false);
            break;
        case Record::HETATM:
            read_ATOM(frame, line, true);
            break;
        case Record::CONECT:
            read_CONECT(frame, line);
            break;
        case Record::MODEL:
            models_++;
            break;
        case Record::ENDMDL:
            // Check if the next record is an `END` record
            if (!file_->eof()) {
                line = file_->readline();
                file_->seekg(position);
                record = get_record(line);
                if (record == Record::END) {
                    // If this is the case, wait for this next record
                    break;
                }
            }
            // Else we have read a frame
            goto end;
        case Record::END:
            // We have read a frame!
            goto end;
        case Record::IGNORED_:
            break; // Nothing to do
        case Record::UNKNOWN_:
            warning("Unknown PDB record: {}", line);
            break;
        }
    }

    // If we are here, we got EOF before an END record
    warning("Missing END record in PDB file");
end:
    for (auto& residue: residues_) {
        frame.add_residue(residue.second);
    }
}

void PDBFormat::read_CRYST1(Frame& frame, const std::string& line) {
    assert(line.substr(0, 6) == "CRYST1");
    if (line.length() < 54) {
        throw format_error("CRYST1 record '{}' is too small", line);
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
    } catch (std::invalid_argument&) {
        throw format_error("could not read CRYST1 record '{}'", line);
    }

    if (line.length() >= 55) {
        auto space_group = trim(line.substr(55, 10));
        if (space_group != "P 1" && space_group != "P1") {
            warning(
                "Space group which is not P1 ({}) ignored in '{}'",
                space_group, file_->filename()
            );
        }
    }
}

void PDBFormat::read_ATOM(Frame& frame, const std::string& line,
    bool is_hetatm) {
    assert(line.substr(0, 6) == "ATOM  " || line.substr(0, 6) == "HETATM");

    if (line.length() < 54) {
        throw format_error(
            "{} record is too small: '{}'", line.substr(0, 6), line
        );
    }

    auto atom = Atom(trim(line.substr(12, 4)));
    if (line.length() >= 78) {
        atom.set_type(trim(line.substr(76, 2)));
    }

    atom.set("is_hetatm", is_hetatm);

    try {
        auto x = std::stof(line.substr(31, 8));
        auto y = std::stof(line.substr(38, 8));
        auto z = std::stof(line.substr(46, 8));

        frame.add_atom(std::move(atom), Vector3D(x, y, z));
    } catch (std::invalid_argument&) {
        throw format_error("could not read positions in '{}'", line);
    }

    auto atom_id = frame.size() - 1;
    try {
        auto resid = std::stoul(line.substr(22, 4));
        if (residues_.find(resid) == residues_.end()) {
            auto name = trim(line.substr(17, 3));
            Residue residue(std::move(name), resid);
            residue.add_atom(atom_id);
            residues_.insert({resid, residue});
        } else {
            // Just add this atom to the residue
            residues_.at(resid).add_atom(atom_id);
        }
    } catch (std::invalid_argument&) {
        // No residue information
    }

}

void PDBFormat::read_CONECT(Frame& frame, const std::string& line) {
    assert(line.substr(0, 6) == "CONECT");
    auto line_length = trim(line).length();

    // Helper lambdas
    auto add_bond = [&frame, &line](size_t i, size_t j) {
        if (i >= frame.size() || j >= frame.size()) {
            warning("Bad atomic numbers in CONECT record, ignored. ({})", line);
            return;
        }
        frame.add_bond(i, j);
    };

    auto read_index = [&line](size_t initial) -> size_t {
        try {
            // PDB indexing is 1-based, and chemfiles is 0-based
            return std::stoul(line.substr(initial, 5)) - 1;
        } catch (std::invalid_argument&) {
            throw format_error("could not read atomic number in '{}'", line);
        }
    };

    auto i = read_index(6);

    if (line_length > 11) {
        auto j = read_index(11);
        add_bond(i, j);
    } else {
        return;
    }

    if (line_length > 16) {
        auto j = read_index(16);
        add_bond(i, j);
    } else {
        return;
    }

    if (line_length > 21) {
        auto j = read_index(21);
        add_bond(i, j);
    } else {
        return;
    }

    if (line_length > 26) {
        auto j = read_index(26);
        add_bond(i, j);
    } else {
        return;
    }
}

bool forward(TextFile& file) {
    if (!file) {return false;}
    while (true) {
        try {
            auto line = file.readline();

            if (line.substr(0, 6) == "ENDMDL") {
                auto position = file.tellg();
                auto next = file.readline();
                file.seekg(position);
                if (next.substr(0, 3) == "END") {
                    // We found another record starting by END in the next line,
                    // we skip this one and wait for the next one
                    continue;
                }
            }

            if (line.substr(0, 3) == "END") {
                return true;
            }

        } catch (const FileError&) {
            return false;
        }
    }
}

Record get_record(const std::string& line) {
    auto rec = line.substr(0, 6);
    if(rec == "ENDMDL") {
        return Record::ENDMDL;
    } else if(rec.substr(0, 3) == "END") {
        // Handle missing whitespace in END record
        return Record::END;
    } else if (rec == "CRYST1") {
        return Record::CRYST1;
    } else if (rec == "ATOM  ") {
        return Record::ATOM;
    } else if (rec == "HETATM") {
        return Record::HETATM;
    } else if (rec == "CONECT") {
        return Record::CONECT;
    } else if (rec == "MODEL "){
        return Record::MODEL;
    } else if (rec == "REMARK" || rec == "MASTER" || rec == "AUTHOR" ||
               rec == "CAVEAT" || rec == "COMPND" || rec == "EXPDTA" ||
               rec == "KEYWDS" || rec == "OBSLTE" || rec == "SOURCE" ||
               rec == "SPLIT " || rec == "SPRSDE" || rec == "TITLE " ||
               rec == "JRNL  " || rec == "TER   " || rec == "HEADER") {
        return Record::IGNORED_;
    } else {
        return Record::UNKNOWN_;
    }
}

static std::string to_pdb_index(uint64_t i) {
    auto id = i + 1;

    if (id >= 100000) {
        warning("Too many atoms for PDB format, removing atomic id");
        return "*****";
    } else {
        return std::to_string(i + 1);
    }
}

void PDBFormat::write(const Frame& frame) {
    written_ = true;
    fmt::print(*file_, "MODEL {:>4}\n", models_ + 1);

    auto& cell = frame.cell();
    check_values_size(Vector3D(cell.a(), cell.b(), cell.c()), 9, "cell lengths");
    fmt::print(
        *file_,
        // Do not try to guess the space group and the z value, just use the
        // default one.
        "CRYST1{:9.3f}{:9.3f}{:9.3f}{:7.2f}{:7.2f}{:7.2f} P 1           1\n",
        cell.a(), cell.b(), cell.c(), cell.alpha(), cell.beta(), cell.gamma());

    // Only use numbers bigger than the biggest residue id as "resSeq" for
    // atoms without associated residue.
    uint64_t max_resid = 0;
    for (const auto& residue: frame.topology().residues()) {
        auto resid = residue.id();
        if (resid && resid.value() > max_resid) {
            max_resid = resid.value();
        }
    }

    for (size_t i = 0; i < frame.size(); i++) {
        auto& name = frame.topology()[i].name();
        auto& type = frame.topology()[i].type();
        auto& pos = frame.positions()[i];

        std::string atom_hetatm = "HETATM";
        auto is_hetatm = frame.topology()[i].get("is_hetatm");
        if (is_hetatm) {
            if (is_hetatm->get_kind() == Property::BOOL) {
                if (is_hetatm->as_bool()) {
                    atom_hetatm = "HETATM";
                } else {
                    atom_hetatm = "ATOM  ";
                }
            } else {
                warning(
                    "\'is_hetatm\' property is not a boolean in PDB writer, using HETATM"
                );
            }
        }

        std::string resname;
        std::string resid;
        auto residue = frame.topology().residue_for_atom(i);
        if (residue) {
            resname = residue->name();
            if (resname.length() > 3) {
                warning(
                    "Residue '{}' has a name too long for PDB format, it will be truncated.",
                    resname
                );
                resname = resname.substr(0, 3);
            }

            if (residue->id()) {
                auto value = residue->id().value();
                if (value > 9999) {
                    warning("Too many residues for PDB format, removing residue id");
                    resid = "  -1";
                } else {
                    resid = std::to_string(residue->id().value());
                }
            } else {
                resid = "  -1";
            }
        }
        else {
            resname = "XXX";
            auto value = max_resid++;
            if (value < 9999) {
                resid = to_pdb_index(value);
            } else {
                resid = "  -1";
            }
        }

        assert(resname.length() <= 3);
        check_values_size(pos, 8, "atomic position");

        // Print all atoms as HETATM, because there is no way we can know if we
        // are handling a biomolecule or not.
        //
        // We ignore the 'altLoc' and 'iCode' fields, as we do not
        // know them.
        //
        // 'chainID' is set to be 'X', and if there is no residue information
        // 'resSeq' to be the atomic number.
        fmt::print(
            *file_,
            "{: <6}{: >5} {: <4s} {:3} X{: >4s}    {:8.3f}{:8.3f}{:8.3f}{:6.2f}{:6.2f}          {: >2s}\n",
            atom_hetatm, to_pdb_index(i), name, resname, resid, pos[0], pos[1], pos[2], 1.0, 0.0, type
        );
    }

    auto connect = std::vector<std::vector<size_t>>(frame.size());
    for (auto& bond : frame.topology().bonds()) {
        if (bond[0] > 99999 || bond[1] > 99999) {
            warning("Atomic index is too big for CONNECT, removing the bond");
            continue;
        }
        connect[bond[0]].push_back(bond[1]);
        connect[bond[1]].push_back(bond[0]);
    }

    for (size_t i = 0; i < frame.size(); i++) {
        auto connections = connect[i].size();
        auto lines = connections / 4 + 1;
        if (connections == 0) {continue;}

        for (size_t conect_line = 0; conect_line < lines; conect_line++) {
            fmt::print(*file_, "CONECT{: >5}", to_pdb_index(i));
            auto last = std::min(connections, 4 * (conect_line + 1));
            for (size_t j = 4 * conect_line; j < last; j++) {
                fmt::print(*file_, "{: >5}", to_pdb_index(connect[i][j]));
            }
            fmt::print(*file_, "\n");
        }
    }

    fmt::print(*file_, "ENDMDL\n");

    models_++;
    steps_positions_.push_back(file_->tellg());
}

void check_values_size(const Vector3D& values, unsigned width, const std::string& context) {
    double max_pos = std::pow(10.0, width) - 1;
    double max_neg = -std::pow(10.0, width - 1) + 1;
    if (values[0] > max_pos || values[1] > max_pos || values[2] > max_pos ||
        values[0] < max_neg || values[1] < max_neg || values[2] < max_neg) {
        throw format_error(
            "value in {} is too big for representation in PDB format", context
        );
    }
}

PDBFormat::~PDBFormat() noexcept {
    if (written_) {
      fmt::print(*file_, "END\n");
    }
}
