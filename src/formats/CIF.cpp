// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/CIF.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

#include "chemfiles/pdb_connectivity.hpp"

// WARNING UGLY HACK!
#include "chemfiles/formats/PDB.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<CIFFormat>() {
    return FormatInfo("CIF").with_extension(".cif").description(
        "PDB (RCSB Protein Data Bank) text format"
    );
}

/// CIF files store which digits are insignificant, we need to remove this
static double cif_to_double(std::string line);

CIFFormat::CIFFormat(std::string path, File::Mode mode, File::Compression compression)
  : file_(TextFile::open(std::move(path), mode, compression)), models_(0) {

    if (mode == File::APPEND || mode == File::WRITE) {
        throw file_error("Cannot Append CIF file");
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

        if (in_loop && line_split[0].find("_atom_site") != std::string::npos) {
            auto atom_label = line_split[0].substr(11);

            std::transform(atom_label.begin(), atom_label.end(),
                atom_label.begin(), std::tolower);

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

            std::transform(atom_label.begin(), atom_label.end(),
                atom_label.begin(), std::tolower);

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
    size_t last_position = std::stoul(split(line, ' ')[model_position->second]);

    do {
        position = file_->tellg();
        line = file_->readline();

        // a break in the text ends the models
        if (!line.size() || line == "line_" || line[0] == '#') {
            break;
        }

        auto line_split = split(line, ' ');
        size_t current_position = std::stoul(line_split[model_position->second]);

        if (current_position != last_position) {
            steps_positions_.push_back(position);
            last_position = current_position;
        }
    } while (!file_->eof());

    file_->seekg(steps_positions_[0]);
}

size_t CIFFormat::nsteps() {
    return steps_positions_.size();
}

void CIFFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

void CIFFormat::read(Frame& frame) {
    frame.resize(0);
    residues_.clear();

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
        last_position = std::stoul(split(line, ' ')[model_position->second]);
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
            current_position = std::stoul(line_split[model_position->second]);
        }

        if (current_position != last_position) {
            break;
        }

        Atom atom(line_split[label_atom_id->second],
                  line_split[type_symbol->second]);

        if (group_pdb != atom_site_map_.end()) {
            atom.set("is_hetatm", line_split[group_pdb->second] != "ATOM");
        }

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
        size_t resid;
        auto resid_text = line_split[label_seq_id->second];

        try {
            if (resid_text == ".") { // In this case, we need to use the entity id
                resid = std::stoul(line_split[label_entity_id->second]);
            } else {
                resid = std::stoul(line_split[label_seq_id->second]);
            }
        } catch (std::invalid_argument& e) {
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

            residues_.insert({{chainid, resid}, residue});
        } else {
            // Just add this atom to the residue
            residues_.at({chainid, resid}).add_atom(atom_id);
        }
    }

    file_->seekg(position);

    for (const auto& residue: residues_) {
        frame.add_residue(residue.second);
    }

    // Cross format talk! Forgive me!
    PDBFormat::link_standard_residue_bonds(frame);
}

void CIFFormat::write(const Frame& frame) {
    throw error("not implemented.");
}

double cif_to_double(std::string line) {
    line.erase(std::remove(line.begin(), line.end(), '('), line.end());
    line.erase(std::remove(line.begin(), line.end(), ')'), line.end());
    return std::stod(line);
}

CIFFormat::~CIFFormat() noexcept {
    if (written_) {
    }
}
