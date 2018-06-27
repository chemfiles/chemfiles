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
    auto line = trim(file_->readline());
    if (line != "@<TRIPOS>MOLECULE") {
        throw format_error("Wrong starting line for a molecule in MOL2 formart: '{}'", line);
    }

    frame.set("name", trim(file_->readline()) );
    line = file_->readline();

    const auto counts = split(line, ' ');

    long long natoms = string2longlong(counts[0]);
    long long nbonds = 0;

    if (counts.size() >= 2) {
        nbonds = string2longlong(counts[1]);
    }

    residues_.clear();
    frame.resize(0);
    frame.reserve(static_cast<size_t>(natoms));

    // Skip a line
    file_->readline();

    // If charges are specified, we need to expect an addition term for each atom
    bool charges = (trim(file_->readline()) != "NO_CHARGES");

    while (!file_->eof()) {
        const auto& curr_pos = file_->tellg();
        line = trim(file_->readline());

        if (line == "@<TRIPOS>ATOM") {
            read_atoms(frame, static_cast<size_t>(natoms), charges);
        } else if (line == "@<TRIPOS>BOND") {
            read_bonds(frame, static_cast<size_t>(nbonds));
        } else if (line == "@<TRIPOS>CRYSIN") {
            auto cryst = file_->readline();

            double a, b, c, alpha, beta, gamma;
            scan(cryst, " %lf %lf %lf %lf %lf %lf",
                &a, &b, &c, &alpha, &beta, &gamma
            );

            frame.set_cell(UnitCell(a, b, c, alpha, beta, gamma));
        } else if (line == "@<TRIPOS>MOLECULE") {
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

        auto atom = Atom(atom_name, atom_type);
        if (charges) {
            atom.set_charge(charge);
        }
        frame.add_atom(std::move(atom), {x, y, z});

        size_t current_atom = frame.size() - 1;
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
        char bond_order[32] = {0};

        scan(line, " %lu %lu %lu %31s", &id, &id_1, &id_2, &bond_order[0]);

        // MOL2 is 1 index-based, not 0
        --id_1;
        --id_2;

        if (id_1 >= frame.size() || id_2 >= frame.size()) {
            throw format_error("Connect ID '{}' or '{}' larger than number of atoms '{}'",
                id_1, id_2, frame.size()
            );
        }

        Bond::BondOrder bo;
        std::string bond_order_str(bond_order);

        if (bond_order_str == "1") {
            bo = Bond::SINGLE;
        } else if (bond_order_str == "2") {
            bo = Bond::DOUBLE;
        } else if (bond_order_str == "3") {
            bo = Bond::TRIPLE;
        } else if (bond_order_str == "ar"){
            bo = Bond::AROMATIC;
        } else if (bond_order_str == "am"){
            bo = Bond::AMIDE;
        } else if (bond_order_str == "du"){ // du is a dummy bond
            bo = Bond::UNKNOWN;
        } else {
            bo = Bond::UNKNOWN;
        }

        frame.add_bond(id_1, id_2, bo);
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

            if (nbonds < 0) {
                throw format_error(
                    "number of bonds can not be negative in '{}'", file.filename()
                );
            }

            read_until(file, "@<TRIPOS>BOND");
            file.readlines(static_cast<size_t>(nbonds));

            return pos;
        } catch (const FileError&) {
            return std::streampos(-1);
        }
    }

    return std::streampos(-1);
}

void MOL2Format::write(const Frame& frame) {
    fmt::print(*file_, "@<TRIPOS>MOLECULE\n");

    const auto& frame_name = frame.get("name");
    if (frame_name && frame_name->get_kind() == Property::STRING) {
        fmt::print(*file_, frame_name->as_string());
    }
    fmt::print(*file_, "\n");

    // Start after the maximal residue id for atoms without associated residue
    uint64_t max_resid = 0;
    for (const auto& residue: frame.topology().residues()) {
        auto resid = residue.id();
        if (resid && resid.value() > max_resid) {
            max_resid = resid.value();
        }
    }

    const auto& bonds = frame.topology().bonds();

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

    auto& bond_orders = frame.topology().bond_orders();

    for (size_t i = 0; i < bonds.size(); i++) {

        std::string bond_order;
        switch (bond_orders[i]) {
            case Bond::SINGLE:
                bond_order = "1";
                break;
            case Bond::DOUBLE:
                bond_order = "2";
                break;
            case Bond::TRIPLE:
                bond_order = "3";
                break;
            case Bond::AROMATIC:
                bond_order = "ar";
                break;
            case Bond::AMIDE:
                bond_order = "am";
                break;
            case Bond::UNKNOWN:
            default:
                bond_order = "du";
                break;
        }

        fmt::print(*file_, "{:4d}  {:4d}  {:4d}    {}\n",
            i + 1, bonds[i][0] + 1, bonds[i][1] + 1, bond_order
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
