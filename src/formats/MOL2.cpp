// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cctype>
#include <cstdint>

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Connectivity.hpp"

#include "chemfiles/utils.hpp"
#include "chemfiles/parse.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/string_view.hpp"
#include "chemfiles/periodic_table.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/formats/MOL2.hpp"

using namespace chemfiles;
template<> FormatInfo chemfiles::format_information<MOL2Format>() {
    return FormatInfo("MOL2").with_extension(".mol2").description(
        "Tripos mol2 text format"
    );
}

/// Fast-forward the file until the tag is found.
static uint64_t read_until(TextFile& file, string_view tag);

void MOL2Format::read_next(Frame& frame) {
    residues_.clear();
    auto line = file_.readline();
    if (trim(line) != "@<TRIPOS>MOLECULE") {
        throw format_error("wrong starting line for a molecule in MOL2 formart: '{}'", line);
    }

    line = file_.readline();
    frame.set("name", trim(line).to_string());
    line = file_.readline();

    const auto counts = split(line, ' ');

    auto natoms = parse<size_t>(counts[0]);
    size_t nbonds = 0;

    if (counts.size() >= 2) {
        nbonds = parse<size_t>(counts[1]);
    }


    file_.readline();
    frame.reserve(natoms);
    // If charges are specified, we need to expect an addition term for each atom
    line = file_.readline();
    bool charges = (trim(line) != "NO_CHARGES");

    while (!file_.eof()) {
        const auto& curr_pos = file_.tellpos();

        line = file_.readline();
        auto trimed = trim(line);

        if (trimed == "@<TRIPOS>ATOM") {
            read_atoms(frame, natoms, charges);
        } else if (trimed == "@<TRIPOS>BOND") {
            read_bonds(frame, nbonds);
        } else if (trimed == "@<TRIPOS>CRYSIN") {
            auto cryst = file_.readline();

            double a, b, c, alpha, beta, gamma;
            scan(cryst, a, b, c, alpha, beta, gamma);

            frame.set_cell(UnitCell(a, b, c, alpha, beta, gamma));
        } else if (trimed == "@<TRIPOS>MOLECULE") {
            file_.seekpos(curr_pos);
            break;
        }
    }

    for (auto& residue: residues_) {
        frame.add_residue(residue.second);
    }

}

void MOL2Format::read_atoms(Frame& frame, size_t natoms, bool charges) {
    for (size_t i=0; i<natoms; i++) {
        auto line = file_.readline();

        unsigned long id, resid;
        std::string atom_name, sybyl_type, resname;
        double x, y, z;
        double charge = 0;

        if (charges) {
            scan(line, id, atom_name, x, y, z, sybyl_type, resid, resname, charge);
        } else {
            scan(line, id, atom_name, x, y, z, sybyl_type, resid, resname);
        }

        std::string atom_type;
        bool is_sybyl;

        if (sybyl_type.find('.') != std::string::npos || find_in_periodic_table(sybyl_type)) {
            atom_type = split(sybyl_type, '.')[0].to_string();
            is_sybyl = true;
        } else {
            is_sybyl = false;
            for (auto c: atom_name) {
                if ((std::isalpha(c) == 0) || !find_in_periodic_table(atom_type + c)) {
                    break;
                }
                atom_type += c;
            }
            warning("[MOL2 reader] invalid sybyl type: '{}'; guessing '{}' from '{}'",
                sybyl_type, atom_type, atom_name
            );
        }

        auto atom = Atom(std::move(atom_name), std::move(atom_type));
        if (charges) {
            atom.set_charge(charge);
        }
        if (is_sybyl) {
            atom.set("sybyl", std::move(sybyl_type));
        }
        frame.add_atom(std::move(atom), {x, y, z});

        size_t current_atom = frame.size() - 1;
        if (residues_.find(resid) == residues_.end()) {
            Residue residue(std::move(resname), resid);
            residue.add_atom(current_atom);
            residues_.insert({resid, residue});
        } else {
            // Just add this atom to the residue
            residues_.at(resid).add_atom(current_atom);
        }

    }
}

void MOL2Format::read_bonds(Frame& frame, size_t nbonds) {
    for (size_t i=0; i<nbonds; i++) {
        auto line = file_.readline();

        unsigned long id, id_1, id_2;
        std::string bond_order;
        scan(line, id, id_1, id_2, bond_order);

        // MOL2 is 1 index-based, not 0
        id_1 -= 1;
        id_2 -= 1;

        if (id_1 >= frame.size() || id_2 >= frame.size()) {
            throw format_error("connect ID '{}' or '{}' larger than number of atoms '{}'",
                id_1, id_2, frame.size()
            );
        }

        Bond::BondOrder order;
        if (bond_order == "1") {
            order = Bond::SINGLE;
        } else if (bond_order == "2") {
            order = Bond::DOUBLE;
        } else if (bond_order == "3") {
            order = Bond::TRIPLE;
        } else if (bond_order == "ar") {
            order = Bond::AROMATIC;
        } else if (bond_order == "am") {
            order = Bond::AMIDE;
        } else if (bond_order == "du") { // du is a dummy bond
            order = Bond::UNKNOWN;
        } else {
            order = Bond::UNKNOWN;
        }

        frame.add_bond(id_1, id_2, order);
    }
}

uint64_t read_until(TextFile& file, string_view tag) {
    while (!file.eof()) {
        auto pos = file.tellpos();
        if (file.readline().substr(0, tag.length()) == tag) {
            return pos;
        }
    }

    throw file_error("file ended before tag '{}' was found", tag);
}

optional<uint64_t> MOL2Format::forward() {
    while (!file_.eof()) {
        try {
            auto position = read_until(file_, "@<TRIPOS>MOLECULE");
            file_.readline();
            auto line = file_.readline();

            const auto counts = split(line, ' ');
            auto natoms = parse<size_t>(counts[0]);
            size_t nbonds = 0;
            if (counts.size() >= 2) {
                nbonds = parse<size_t>(counts[1]);
            }

            read_until(file_, "@<TRIPOS>ATOM");
            for (size_t i=0; i<natoms; i++) {
                file_.readline();
            }

            read_until(file_, "@<TRIPOS>BOND");
            for (size_t i=0; i<nbonds; i++) {
                file_.readline();
            }

            return position;
        } catch (const Error&) {
            return nullopt;
        }
    }

    return nullopt;
}

void MOL2Format::write_next(const Frame& frame) {
    file_.print("@<TRIPOS>MOLECULE\n");
    file_.print("{}\n", frame.get<Property::STRING>("name").value_or(""));

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
    file_.print("{:4d}  {:4d}    1    0    0\n",
        frame.size(), bonds.size()
    );

    file_.print("SMALL\nUSER_CHARGES\n\n@<TRIPOS>ATOM\n");

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

        std::string sybyl;
        bool has_sybyl = true;
        if (frame[i].get("sybyl") && frame[i].get("sybyl")->kind() == Property::STRING) {
            sybyl = frame[i].get("sybyl")->as_string();
        } else {
            has_sybyl = false;
            sybyl = frame[i].type();
        }

        if (!has_sybyl) {
            warning("Sybyl type not set. Using element type instead");
        }

        file_.print(
            "{:4d} {:4s}  {:.6f} {:.6f} {:.6f} {:s} {} {} {:.6f}\n",
            i + 1, frame[i].name(), positions[i][0], positions[i][1], positions[i][2], sybyl, resid, resname, frame[i].charge()
        );
    }

    file_.print("@<TRIPOS>BOND\n");

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

        file_.print("{:4d}  {:4d}  {:4d}    {}\n",
            i + 1, bonds[i][0] + 1, bonds[i][1] + 1, bond_order
        );
    }

    auto cell = frame.cell();
    if (cell.shape() != UnitCell::INFINITE) {
        file_.print("@<TRIPOS>CRYSIN\n");
        file_.print("   {:.4f}   {:.4f}   {:.4f}   {:.4f}   {:.4f}   {:.4f} 1 1\n",
            cell.a(), cell.b(), cell.c(), cell.alpha(), cell.beta(), cell.gamma()
        );
    }

    file_.print("@<TRIPOS>SUBSTRUCTURE\n");
    file_.print("   1 ****        1 TEMP                        ");
    file_.print("0 ****  **** 0 ROOT\n\n");
}
