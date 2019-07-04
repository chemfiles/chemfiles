// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <map>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <istream>
#include <utility>
#include <algorithm>

#include <fmt/ostream.h>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/external/optional.hpp"

// WARNING UGLY HACK!
#include "chemfiles/formats/PDB.hpp"

#include "chemfiles/formats/mmCIF.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<mmCIFFormat>() {
    return FormatInfo("mmCIF").with_extension(".mmcif").description(
        "mmCIF (Crystallographic Information Framework) for MacroMolecules"
    );
}

/// CIF files store which digits are insignificant, we need to remove this
static double cif_to_double(std::string line);

mmCIFFormat::mmCIFFormat(std::string path, File::Mode mode, File::Compression compression)
  : file_(TextFile::open(std::move(path), mode, compression)), models_(0), atoms_(0) {

    if (mode == File::WRITE) {
        return;
    }

    if (mode == File::APPEND) {
        throw file_error("Cannot Append mmCIF file");
    }

    double a = 0;
    double b = 0;
    double c = 0;
    double alpha = 0;
    double beta = 0;
    double gamma = 0;

    bool in_loop = false;
    size_t current_index = 0;

    while (!file_->eof()) {
        auto line = file_->readline();

        if (line.find("loop_") != std::string::npos) {
            in_loop = true;
            continue;
        }

        if (!line.size() || line[0] == '#') {
            in_loop = false;
            continue;
        }

        auto line_split = split(line, ' ');
        if (line_split.size() > 1 && line[0] == '_') {
            in_loop = false;
        }

        if (line_split[0] == "_cell_length_a") {
            a = cif_to_double(line_split[1]);
        }

        if (line_split[0] == "_cell_length_b") {
            b = cif_to_double(line_split[1]);
        }

        if (line_split[0] == "_cell_length_c") {
            c = cif_to_double(line_split[1]);
        }

        if (line_split[0] == "_cell_angle_alpha") {
            alpha = cif_to_double(line_split[1]);
        }

        if (line_split[0] == "_cell_angle_beta") {
            beta = cif_to_double(line_split[1]);
        }

        if (line_split[0] == "_cell_angle_gamma") {
            gamma = cif_to_double(line_split[1]);
        }

        if (line_split[0] == "_entry.id") {
            pdb_idcode_ = line_split[1];
        }

        if (line_split[0] == "_struct.title") {
            auto temp_name = trim(line.substr(13));
            name_ = temp_name.size() > 2 ?
                    temp_name.substr(1, temp_name.size() - 2) : "";
        }

        if (in_loop && line_split[0].find("_atom_site") != std::string::npos) {
            auto atom_label = line_split[0].substr(11);
            tolower(atom_label);
            atom_site_map_[atom_label] = current_index++;
            break;
        }
    }

    if (!current_index) {
        throw format_error("No atom sites!");
    }

    cell_ = UnitCell(a, b, c, alpha, beta, gamma);

    auto position = file_->tellg();
    auto line = file_->readline();

    do {

        if (line.find("_atom_site") != std::string::npos) {
            auto atom_label = trim(line).substr(11);
            tolower(atom_label);
            atom_site_map_[atom_label] = current_index++;

            position = file_->tellg();
            line = file_->readline();

            continue;
        }

        // Anything else ends the block
        break;
    } while (!file_->eof());

    // After this block ends, we have the start of coordinates
    steps_positions_.push_back(position);

    if (atom_site_map_.find("type_symbol") == atom_site_map_.end()) {
        throw format_error("CIF files does not define type symbol");
    }

    // Attempt to guess how coordinates are stored
    auto cartn_x = atom_site_map_.find("cartn_x");
    auto fract_x = atom_site_map_.find("fract_x");
    if (cartn_x != atom_site_map_.end()) {
        uses_fract_ = false;
    } else if (fract_x != atom_site_map_.end()) {
        uses_fract_ = true;
    } else {
        throw format_error("CIF file does not define coordinates!");
    }


    auto model_position = atom_site_map_.find("pdbx_pdb_model_num");
    // Do we have a special extension for multiple modes?
    if (model_position == atom_site_map_.end()) {
        // If not, we are done
        file_->seekg(steps_positions_[0]);
        return;
    }

    // Ok, let's look at the sites now to note where models start
    auto last_position = parse<size_t>(split(line, ' ')[model_position->second]);

    do {
        position = file_->tellg();
        line = file_->readline();

        // a break in the text ends the models
        if (!line.size() || line == "line_" || line[0] == '#') {
            break;
        }

        auto line_split = split(line, ' ');
        size_t current_position = parse<size_t>(line_split[model_position->second]);

        if (current_position != last_position) {
            steps_positions_.push_back(position);
            last_position = current_position;
        }
    } while (!file_->eof());

    file_->seekg(steps_positions_[0]);
}

size_t mmCIFFormat::nsteps() {
    return steps_positions_.size();
}

void mmCIFFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

void mmCIFFormat::read(Frame& frame) {
    frame.resize(0);
    residues_.clear();

    if (name_ != "") {
        frame.set("name", name_);
    }

    if (pdb_idcode_ != "") {
        frame.set("pdb_idcode", pdb_idcode_);
    }

    // The following map operations can be moved to the constructor
    // and stored with optional. I don't know which solution is better.

    // These are required for atoms
    auto type_symbol = atom_site_map_.find("type_symbol");

    // This has two names...
    auto label_atom_id = atom_site_map_.find("label_atom_id");
    if (label_atom_id == atom_site_map_.end()) {
        label_atom_id = atom_site_map_.find("label");
    }

    // Other atom properties
    auto group_pdb = atom_site_map_.find("group_pdb");
    auto label_alt_id = atom_site_map_.find("label_alt_id");
    auto formal_charge = atom_site_map_.find("formal_charge");

    // Positions
    auto cartn_x = atom_site_map_.find("cartn_x");
    auto cartn_y = atom_site_map_.find("cartn_y");
    auto cartn_z = atom_site_map_.find("cartn_z");

    // Positions used by COD and CCSD
    auto fract_x = atom_site_map_.find("fract_x");
    auto fract_y = atom_site_map_.find("fract_y");
    auto fract_z = atom_site_map_.find("fract_z");

    // Residue properties
    auto label_comp_id = atom_site_map_.find("label_comp_id");
    auto label_asym_id = atom_site_map_.find("label_asym_id");
    auto auth_asym_id = atom_site_map_.find("auth_asym_id");
    auto label_seq_id = atom_site_map_.find("label_seq_id");
    auto label_entity_id = atom_site_map_.find("label_entity_id");

    auto model_position = atom_site_map_.find("pdbx_pdb_model_num");

    auto position = file_->tellg();
    auto line = file_->readline();

    size_t last_position = 0;
    if (model_position != atom_site_map_.end()) {
        last_position = parse<size_t>(split(line, ' ')[model_position->second]);
    }

    while (!file_->eof()) {

        if (!line.size() || line == "loop_" || line[0] == '#') {
            break;
        }

        auto line_split = split(line, ' ');

        if (line_split.size() != atom_site_map_.size()) {
            throw format_error("Line '{}' has {} items not {}",
                line, line_split.size(), atom_site_map_.size()
            );
        }

        size_t current_position = 0;
        if (model_position != atom_site_map_.end()) {
            current_position = parse<size_t>(line_split[model_position->second]);
        }

        if (current_position != last_position) {
            break;
        }

        Atom atom(line_split[label_atom_id->second],
                  line_split[type_symbol->second]);

        if (label_alt_id != atom_site_map_.end() &&
            line_split[label_alt_id->second] != ".") {
            atom.set("altloc", line_split[label_alt_id->second]);
        }

        if (formal_charge != atom_site_map_.end()) {
            atom.set_charge(cif_to_double(line_split[formal_charge->second]));
        }

        if (!uses_fract_) {
            auto x = cif_to_double(line_split[cartn_x->second]);
            auto y = cif_to_double(line_split[cartn_y->second]);
            auto z = cif_to_double(line_split[cartn_z->second]);

            frame.add_atom(std::move(atom), Vector3D(x, y, z));
        } else {
            auto u = cif_to_double(line_split[fract_x->second]);
            auto v = cif_to_double(line_split[fract_y->second]);
            auto w = cif_to_double(line_split[fract_z->second]);

            auto vector = cell_.matrix() * Vector3D(u, v, w);
            frame.add_atom(std::move(atom), vector);
        }

        position = file_->tellg();
        line = file_->readline();

        if (label_comp_id == atom_site_map_.end() ||
            label_asym_id == atom_site_map_.end()) {
            continue;
        }

        auto atom_id = frame.size() - 1;
        size_t resid = 0;
        auto resid_text = line_split[label_seq_id->second];

        try {
            if (resid_text == ".") { // In this case, we need to use the entity id
                resid = parse<size_t>(line_split[label_entity_id->second]);
            } else {
                resid = parse<size_t>(line_split[label_seq_id->second]);
            }
        } catch (const Error& e) {
            throw format_error("Invalid CIF residue or entity numeric: {}", e.what());
        }

        auto chainid = line_split[label_asym_id->second];

        if (residues_.find({chainid, resid}) == residues_.end()) {

            auto name = line_split[label_comp_id->second];
            Residue residue(std::move(name), resid);
            residue.add_atom(atom_id);

            // This will be save as a string... on purpose to match MMTF
            if (label_asym_id != atom_site_map_.end()) {
                residue.set("chainid", chainid);
            }

            if (auth_asym_id != atom_site_map_.end()) {
                residue.set("chainname", line_split[auth_asym_id->second]);
            }

            if (group_pdb != atom_site_map_.end()) {
                residue.set("is_standard_pdb", line_split[group_pdb->second] == "ATOM");
            }

            residues_.insert({{chainid, resid}, std::move(residue)});
        } else {
            // Just add this atom to the residue
            residues_.at({chainid, resid}).add_atom(atom_id);
        }
    }

    file_->seekg(position);

    for (const auto& residue: residues_) {
        frame.add_residue(residue.second);
    }

    // Only link if we are reading mmCIF
    if (model_position != atom_site_map_.end()) {
        // Cross format talk! Forgive me!
        PDBFormat::link_standard_residue_bonds(frame);
    }
}

void mmCIFFormat::write(const Frame& frame) {

    if (!models_) {
        fmt::print(*file_, "# generated by Chemfiles\n");
        fmt::print(*file_, "#\n");
        fmt::print(*file_, "_cell.length_a {}\n", frame.cell().a());
        fmt::print(*file_, "_cell.length_b {}\n", frame.cell().b());
        fmt::print(*file_, "_cell.length_c {}\n", frame.cell().c());
        fmt::print(*file_, "_cell.length_alpha {}\n", frame.cell().alpha());
        fmt::print(*file_, "_cell.length_beta  {}\n", frame.cell().beta());
        fmt::print(*file_, "_cell.length_gamma {}\n", frame.cell().gamma());
        fmt::print(*file_, "#\n");
        fmt::print(*file_, "loop_\n");
        fmt::print(*file_, "_atom_site.group_PDB\n");
        fmt::print(*file_, "_atom_site.id\n");
        fmt::print(*file_, "_atom_site.type_symbol\n");
        fmt::print(*file_, "_atom_site.label_atom_id\n");
        fmt::print(*file_, "_atom_site.label_alt_id\n");
        fmt::print(*file_, "_atom_site.label_comp_id\n");
        fmt::print(*file_, "_atom_site.label_asym_id\n");
        fmt::print(*file_, "_atom_site.label_seq_id\n");
        fmt::print(*file_, "_atom_site.Cartn_x\n");
        fmt::print(*file_, "_atom_site.Cartn_y\n");
        fmt::print(*file_, "_atom_site.Cartn_z\n");
        fmt::print(*file_, "_atom_site.pdbx_formal_charge\n");
        fmt::print(*file_, "_atom_site.auth_asym_id\n");
        fmt::print(*file_, "_atom_site.pdbx_PDB_model_num\n");
    }

    models_++;

    const auto& topology = frame.topology();
    const auto positions = frame.positions();
    for (size_t i = 0; i < frame.size(); ++i) {
        ++atoms_;

        std::string compid = ".";
        std::string asymid = ".";
        std::string seq_id = ".";
        std::string auth_asymid = ".";
        std::string pdbgroup = "HETATM";

        const auto& residue = topology.residue_for_atom(i);
        if (residue) {
            compid = residue->name();

            if (residue->id()) {
                seq_id = std::to_string(*residue->id());
            } else {
                seq_id = "?";
            }

            asymid = residue->get<Property::STRING>("chainid").value_or("?");
            auth_asymid = residue->get<Property::STRING>("chainname").value_or(".");
            if (residue->get<Property::BOOL>("is_standard_pdb").value_or(false)) {
                pdbgroup = "ATOM  ";
            }
        }

        const auto& atom = frame[i];

        fmt::print(*file_, "{} {: <5} {: <2} {: <4} {} {: >3} {} {: >4} {:8.3f} {:8.3f} {:8.3f} {} {} {}\n",
                pdbgroup, atoms_, atom.type(), atom.name(), ".", compid,
                asymid, seq_id, positions[i][0], positions[i][1], positions[i][2],
                atom.charge(), auth_asymid, models_
        );
    }

}

double cif_to_double(std::string line) {
    line.erase(std::remove(line.begin(), line.end(), '('), line.end());
    line.erase(std::remove(line.begin(), line.end(), ')'), line.end());
    return parse<double>(line);
}
