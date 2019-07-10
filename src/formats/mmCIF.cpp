// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <cstdint>
#include <map>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <algorithm>

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
#include "chemfiles/parse.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/string_view.hpp"
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
  : file_(std::move(path), mode, compression), models_(0), atoms_(0) {

    if (mode == File::WRITE) {
        return;
    }

    if (mode == File::APPEND) {
        throw file_error("cannot Append mmCIF file");
    }

    double a = 0;
    double b = 0;
    double c = 0;
    double alpha = 90;
    double beta = 90;
    double gamma = 90;

    bool in_loop = false;
    size_t current_index = 0;

    while (!file_.eof()) {
        auto line = file_.readline();

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

        if (line_split[0] == "_cell_length_a" || line_split[0] == "_cell.length_a") {
            a = cif_to_double(line_split[1].to_string());
        }

        if (line_split[0] == "_cell_length_b" || line_split[0] == "_cell.length_b") {
            b = cif_to_double(line_split[1].to_string());
        }

        if (line_split[0] == "_cell_length_c" || line_split[0] == "_cell.length_c") {
            c = cif_to_double(line_split[1].to_string());
        }

        if (line_split[0] == "_cell_angle_alpha" || line_split[0] == "_cell.angle_alpha") {
            alpha = cif_to_double(line_split[1].to_string());
        }

        if (line_split[0] == "_cell_angle_beta" || line_split[0] == "_cell.angle_beta") {
            beta = cif_to_double(line_split[1].to_string());
        }

        if (line_split[0] == "_cell_angle_gamma" || line_split[0] == "_cell.angle_gamma") {
            gamma = cif_to_double(line_split[1].to_string());
        }

        if (line_split[0] == "_entry.id") {
            pdb_idcode_ = line_split[1].to_string();
        }

        if (line_split[0] == "_struct.title") {
            auto trimed = trim(line.substr(13));
            name_ = trimed.size() > 2 ?
                    trimed.substr(1, trimed.size() - 2).to_string() : "";
        }

        if (in_loop && line_split[0].find("_atom_site") != std::string::npos) {
            auto atom_label = line_split[0].substr(11).to_string();
            tolower(atom_label);
            atom_site_map_[atom_label] = current_index++;
            break;
        }
    }

    if (!current_index) {
        throw format_error("no atom sites");
    }

    cell_ = UnitCell(a, b, c, alpha, beta, gamma);

    auto position = file_.tellpos();
    auto line = file_.readline();

    do {
        if (line.find("_atom_site") != std::string::npos) {
            auto atom_label = trim(line).substr(11).to_string();
            tolower(atom_label);
            atom_site_map_[atom_label] = current_index++;

            position = file_.tellpos();
            line = file_.readline();

            continue;
        }

        // Anything else ends the block
        break;
    } while (!file_.eof());

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
        throw format_error("CIF file does not define coordinates");
    }


    auto model_position = atom_site_map_.find("pdbx_pdb_model_num");
    // Do we have a special extension for multiple modes?
    if (model_position == atom_site_map_.end()) {
        // If not, we are done
        file_.seekpos(steps_positions_[0]);
        return;
    }

    // Ok, let's look at the sites now to note where models start
    auto last_position = parse<size_t>(split(line, ' ')[model_position->second]);

    do {
        position = file_.tellpos();
        line = file_.readline();

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
    } while (!file_.eof());

    file_.seekpos(steps_positions_[0]);
}

size_t mmCIFFormat::nsteps() {
    return steps_positions_.size();
}

void mmCIFFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_.seekpos(steps_positions_[step]);
    read(frame);
}

void mmCIFFormat::read(Frame& frame) {
    residues_.clear();
    frame.set_cell(cell_);

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

    auto position = file_.tellpos();

    size_t last_position = 0;
    if (model_position != atom_site_map_.end()) {
        auto line = file_.readline();
        last_position = parse<size_t>(split(line, ' ')[model_position->second]);
        // Reset file position so that the loop below can start by reading the
        // first line
        file_.seekpos(position);
    }

    while (!file_.eof()) {
        auto line = file_.readline();
        auto line_split = split(line, ' ');
        if (line.empty() || line == "loop_" || line[0] == '#') {
            break;
        }

        if (line_split.size() != atom_site_map_.size()) {
            throw format_error("line '{}' has {} items not {}",
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

        Atom atom(line_split[label_atom_id->second].to_string(),
                  line_split[type_symbol->second].to_string());

        if (label_alt_id != atom_site_map_.end() &&
            line_split[label_alt_id->second] != ".") {
            atom.set("altloc", line_split[label_alt_id->second].to_string());
        }

        if (formal_charge != atom_site_map_.end()) {
            atom.set_charge(cif_to_double(line_split[formal_charge->second].to_string()));
        }

        if (!uses_fract_) {
            auto x = cif_to_double(line_split[cartn_x->second].to_string());
            auto y = cif_to_double(line_split[cartn_y->second].to_string());
            auto z = cif_to_double(line_split[cartn_z->second].to_string());

            frame.add_atom(std::move(atom), Vector3D(x, y, z));
        } else {
            auto u = cif_to_double(line_split[fract_x->second].to_string());
            auto v = cif_to_double(line_split[fract_y->second].to_string());
            auto w = cif_to_double(line_split[fract_z->second].to_string());

            auto vector = cell_.matrix() * Vector3D(u, v, w);
            frame.add_atom(std::move(atom), vector);
        }

        position = file_.tellpos();

        if (label_comp_id == atom_site_map_.end() || label_asym_id == atom_site_map_.end()) {
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
            throw format_error("invalid CIF residue or entity numeric: {}", e.what());
        }

        auto chainid = line_split[label_asym_id->second].to_string();

        if (residues_.find({chainid, resid}) == residues_.end()) {

            auto name = line_split[label_comp_id->second];
            Residue residue(name.to_string(), resid);
            residue.add_atom(atom_id);

            // This will be saved as a string on purpose to match MMTF
            if (label_asym_id != atom_site_map_.end()) {
                residue.set("chainid", chainid);
            }

            if (auth_asym_id != atom_site_map_.end()) {
                residue.set("chainname", line_split[auth_asym_id->second].to_string());
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

    // Reset state to previous line
    file_.seekpos(position);

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
    if (models_ == 0) {
        file_.print("# generated by Chemfiles\n");
        file_.print("#\n");
        file_.print("_cell.length_a {}\n", frame.cell().a());
        file_.print("_cell.length_b {}\n", frame.cell().b());
        file_.print("_cell.length_c {}\n", frame.cell().c());
        file_.print("_cell.length_alpha {}\n", frame.cell().alpha());
        file_.print("_cell.length_beta  {}\n", frame.cell().beta());
        file_.print("_cell.length_gamma {}\n", frame.cell().gamma());
        file_.print("#\n");
        file_.print("loop_\n");
        file_.print("_atom_site.group_PDB\n");
        file_.print("_atom_site.id\n");
        file_.print("_atom_site.type_symbol\n");
        file_.print("_atom_site.label_atom_id\n");
        file_.print("_atom_site.label_alt_id\n");
        file_.print("_atom_site.label_comp_id\n");
        file_.print("_atom_site.label_asym_id\n");
        file_.print("_atom_site.label_seq_id\n");
        file_.print("_atom_site.Cartn_x\n");
        file_.print("_atom_site.Cartn_y\n");
        file_.print("_atom_site.Cartn_z\n");
        file_.print("_atom_site.pdbx_formal_charge\n");
        file_.print("_atom_site.auth_asym_id\n");
        file_.print("_atom_site.pdbx_PDB_model_num\n");
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

        file_.print("{} {: <5} {: <2} {: <4} {} {: >3} {} {: >4} {:8.3f} {:8.3f} {:8.3f} {} {} {}\n",
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
