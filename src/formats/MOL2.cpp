// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/MOL2.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;
template<> FormatInfo chemfiles::format_information<MOL2Format>() {
    return FormatInfo("MOL2").with_extension(".mol2").description(
        "Tripos mol2 text format"
    );
}

/// Fast-forward the file until the tag is found.
static std::streampos read_until(TextFile& file, const std::string& tag);
/// Fast-forward the file for one step, returning a valid positionif the file does
/// contain one more step or -1 if it does not.
static std::streampos forward(TextFile& file);

MOL2Format::MOL2Format(const std::string& path, File::Mode mode)
  : file_(TextFile::create(path, mode)) {
    while (!file_->eof()) {
        auto position = forward(*file_);
        if (position == std::streampos(-1)) {
            break;
        }
        if (!file_) {
            throw format_error("IO error while reading '{}' as MOL2", path);
        }
        steps_positions_.push_back(position);
    }
    file_->rewind();
}

size_t MOL2Format::nsteps() {
    return steps_positions_.size();
}

void MOL2Format::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

void MOL2Format::read(Frame& frame) {
    assert(file_->readline().substr(0, 17) == "@<TRIPOS>MOLECULE");

    frame.set("name", trim(file_->readline()) );
    const auto& line = file_->readline();

    const auto counts = split(line, ' ');

    long long natoms = string2longlong(counts[0]);
    long long nbonds = 0;
    
    if (counts.size() >= 2) {
        nbonds = string2longlong(counts[1]);
    }

    residues_.clear();
    frame.resize(0);
    frame.reserve(static_cast<size_t>(natoms));

    file_->readline();
    bool charges = (trim(file_->readline()) != "NO_CHARGES");

    while (!file_->eof()) {
        auto curr_pos = file_->tellg();
        auto line2 = trim(file_->readline());

        if (line2 == "@<TRIPOS>ATOM") {
            read_atoms(frame, static_cast<size_t>(natoms), charges);
        } else if (line2 == "@<TRIPOS>BOND") {
            read_bonds(frame, static_cast<size_t>(nbonds));
        } else if (line2 == "@<TRIPOS>CRYSIN") {
            auto cryst = file_->readline();

            double a, b, c, alpha, beta, gamma;
            scan(cryst, " %lf %lf %lf %lf %lf %lf",
                &a, &b, &c, &alpha, &beta, &gamma
            );

            frame.set_cell(UnitCell(a, b, c, alpha, beta, gamma));
        } else if (line2 == "@<TRIPOS>MOLECULE") {
            file_->seekg(curr_pos);
            break;
        }
    }

    for (auto& residue: residues_) {
        frame.add_residue(residue.second);
    }

}

void MOL2Format::read_atoms(Frame& frame, size_t natoms, bool charges) {
    auto lines = file_->readlines(natoms);

    for (const auto& line : lines) {
        unsigned long id, resid;
        char atom_name[32], atom_type[32], res_name[32];
        double x, y, z;
        double charge = 0;

        if (charges) {
            scan(line, " %lu %31s %lf %lf %lf %31s %lu %31s %lf",
                &id, &atom_name[0], &x, &y, &z, &atom_type[0], &resid, &res_name[0], &charge
            );
        } else {
            scan(line, " %lu %31s %lf %lf %lf %31s %lu %31s",
                &id, &atom_name[0], &x, &y, &z, &atom_type[0], &resid, &res_name[0]
            );
        }

        Atom atom(atom_name);
        frame.add_atom(std::move(atom), Vector3D(x, y, z));
        size_t current_atom = frame.size() - 1;
        if (charges) {
            frame[current_atom].set_charge(charge);
        }
        frame[current_atom].set_type(atom_type);        

        if (residues_.find(resid) == residues_.end()) {
            Residue residue(res_name, resid);
            residue.add_atom(current_atom);

            residues_.insert({resid, residue});
        } else {
            // Just add this atom to the residue
            residues_.at(resid).add_atom(current_atom);
        }

    }
}

void MOL2Format::read_bonds(Frame& frame, size_t nbonds) {
    auto lines = file_->readlines(nbonds);

    for (const auto& line : lines) {
        unsigned long id, id_1, id_2;

        scan(line, " %lu %lu %lu", &id, &id_1, &id_2);

        // MOL2 is 1 index-based, not 0
        --id_1;
        --id_2;

        if (id_1 >= frame.size() || id_2 >= frame.size()) {
            throw format_error("Connect ID '{}' or '{}' larger than number of atoms '{}'",
                id_1, id_2, frame.size()
            );
        }

        frame.add_bond(id_1, id_2);
    }
}

std::streampos read_until(TextFile& file, const std::string& tag) {
    while (!file.eof()) {
        std::streampos pos = file.tellg();
        if (file.readline().substr(0, tag.length()) == tag) {
            return pos;
        }
    }

    throw file_error("File ended before tag '{}' was found", tag);
}

std::streampos forward(TextFile& file) {
    if (!file) {return std::streampos(-1);}
    while (!file.eof()) {
        try {
            std::streampos pos = read_until(file, "@<TRIPOS>MOLECULE");
            file.readline(); // Skip a line
            auto line = file.readline();

            const auto counts = split(line, ' ');

            long long natoms, nbonds = 0;
            natoms = string2longlong(counts[0]);

            if (natoms < 0) {
                throw format_error(
                    "number of atoms can not be negative in '{}'", file.filename()
                );
            }

            if (counts.size() >= 2) {
                nbonds = string2longlong(counts[1]);
            }

            read_until(file, "@<TRIPOS>ATOM");
            file.readlines(static_cast<size_t>(natoms));

            if (nbonds > 0) {
                read_until(file, "@<TRIPOS>BOND");
                file.readlines(static_cast<size_t>(nbonds));
            }

            return pos;
        } catch (const FileError&) {
            return std::streampos(-1);
        }
    }

    return std::streampos(-1);
}

void MOL2Format::write(const Frame& frame) {
    fmt::print(*file_, "@<TRIPOS>MOLECULE\n");

    if (frame.get("name") && frame.get("name")->get_kind() == Property::STRING) {
        fmt::print(*file_, frame.get("name")->as_string());
    }
    fmt::print(*file_, "\n");

    // Only use numbers bigger than the biggest residue id as "resSeq" for
    // atoms without associated residue.
    uint64_t max_resid = 0;
    for (const auto& residue: frame.topology().residues()) {
        auto resid = residue.id();
        if (resid && resid.value() > max_resid) {
            max_resid = resid.value();
        }
    }

    auto bonds = std::vector<std::pair<size_t, size_t>>();
    for (auto& bond : frame.topology().bonds()) {
        bonds.push_back({bond[0], bond[1]});
    }

    // Basic format taken from VMD Molfiles
    fmt::print(*file_, "{:4d}  {:4d}    1    0    0\n",
        frame.size(), bonds.size()
    );

    fmt::print(*file_, "SMALL\nUSER_CHARGES\n\n@<TRIPOS>ATOM\n");

    auto& positions = frame.positions();
    for (size_t i = 0; i < frame.size(); i++) {

        std::string resname;
        std::string resid;

        auto residue = frame.topology().residue_for_atom(i);
        if (residue) {
            resname = residue->name();

            if (residue->id()) {
                resid = std::to_string(residue->id().value());
            } else {
                resid = std::to_string(++max_resid);
            }
        } else {
            resname = "XXX";
            resid = std::to_string(++max_resid);
        }

        fmt::print(
            *file_,
            "{:4d} {:4s}  {:.6f} {:.6f} {:.6f} {:s} {} {} {:.6f}\n",
            i + 1, frame[i].name(), positions[i][0], positions[i][1], positions[i][2], frame[i].type(), resid, resname, frame[i].charge()
        );
    }

    fmt::print(*file_, "@<TRIPOS>BOND\n");

    // TODO: Implement bond orders
    for (size_t i = 0; i < bonds.size(); i++) {
        fmt::print(*file_, "{:4d}  {:4d}  {:4d}    1\n",
            i + 1, bonds[i].first + 1, bonds[i].second + 1
        );
    }

    auto cell = frame.cell();
    if (cell.shape() != UnitCell::INFINITE) {
        fmt::print(*file_, "@<TRIPOS>CRYSIN\n");
        fmt::print(*file_, "   {:.4f}   {:.4f}   {:.4f}   {:.4f}   {:.4f}   {:.4f} 1 1\n",
            cell.a(), cell.b(), cell.c(), cell.alpha(), cell.beta(), cell.gamma()
        );
    }

    fmt::print(*file_,"@<TRIPOS>SUBSTRUCTURE\n");
    fmt::print(*file_,"   1 ****        1 TEMP                        ");
    fmt::print(*file_,"0 ****  **** 0 ROOT\n\n");
}

MOL2Format::~MOL2Format() noexcept {
}
