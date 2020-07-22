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

#include "chemfiles/formats/CIF.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<CIFFormat>() {
    return FormatInfo("CIF").with_extension(".cif").description(
        "CIF (Crystallographic Information Framework)"
    );
}

void CIFFormat::init_() {
    gemmi::cif::Document *doc;

    if (file_.mode() == File::WRITE) {
        return;
    }

    if (file_.mode() == File::APPEND) {
        throw file_error("cannot open CIF files in append ('a') mode");
    }

    // Parse the CIF file
    auto content = file_.readall();
    try {
	doc = new gemmi::cif::Document(gemmi::cif::read_string(content));
    } catch (std::exception& e) {
	throw format_error("cannot parse CIF file");
    }

    // Interpret the content of each block, to remove blocks that contain
    // no atom: those can occur, but are not useful to us (they contain comments
    // and associated experimental data).
    for (const auto &block: doc->blocks) {
      try {
	gemmi::SmallStructure s = gemmi::make_small_structure_from_block(block);

	if (s.get_all_unit_cell_sites().size() > 0) {
	  structures_.push_back(s);
	}
      } catch (std::exception& e) {
	/* Simply skip to the next block. */
	throw format_error("cannot interpret CIF block");
      }
    }

    // TODO: if the CIF file has zero blocks with atoms, maybe we should throw an error here
    // if (structures_.size() == 0) ...
}

size_t CIFFormat::nsteps() {
    return structures_.size();
}

void CIFFormat::read_step(const size_t step, Frame& frame) {
    assert(step < structures_.size());

    auto structure = structures_[step];
    auto sites = structure.get_all_unit_cell_sites();

    UnitCell cell = UnitCell(structure.cell.a, structure.cell.b, structure.cell.c,
	structure.cell.alpha, structure.cell.beta, structure.cell.gamma);
    frame.set_cell(cell);

    frame.set("name", structure.name);

    for (const auto& site: sites) {
        Atom atom(site.label, site.element.name());
        // TODO: should we use those somehow?
	// site.element.atomic_number()
	// site.element.weight()
	// site.occ is the occupancy

	gemmi::Fractional fract = site.fract.wrap_to_unit();
	gemmi::Position p = structure.cell.orthogonalize(fract);

        frame.add_atom(std::move(atom), Vector3D(p.x, p.y, p.z));
    }
}

// TODO: is it mandatory to implement both read_step() and read()?
void CIFFormat::read(Frame& frame) {
    // TODO
}

void CIFFormat::write(const Frame& frame) {
    file_.print("data_model_{}\n", models_); // TODO: do models have a name in Chemfiles? If so, use it
    file_.print("_audit_creation_method         'generated by Chemfiles'\n");
    file_.print("_symmetry_cell_setting         'triclinic'\n");
    file_.print("_symmetry_space_group_name_H-M 'P 1'\n");
    file_.print("_space_group_IT_number         1\n");
    file_.print("_cell.length_a {}\n", frame.cell().a());
    file_.print("_cell.length_b {}\n", frame.cell().b());
    file_.print("_cell.length_c {}\n", frame.cell().c());
    file_.print("_cell.length_alpha {}\n", frame.cell().alpha());
    file_.print("_cell.length_beta  {}\n", frame.cell().beta());
    file_.print("_cell.length_gamma {}\n", frame.cell().gamma());
    file_.print("loop_\n");
    file_.print("  _symmetry_equiv_pos_as_xyz\n");
    file_.print("  '+x,+y,+z'\n");

    file_.print("\n");
    file_.print("loop_\n");
    file_.print("_atom_site_label\n");
    file_.print("_atom_site_type_symbol\n");
    file_.print("_atom_site_occupancy\n");
    file_.print("_atom_site_fract_x\n");
    file_.print("_atom_site_fract_y\n");
    file_.print("_atom_site_fract_z\n");
    // Cartesian coordinates are rare in CIF file, but allowed. Most programs will ignore them
    // and use fractional coordinates
    file_.print("_atom_site_Cartn_x\n");
    file_.print("_atom_site_Cartn_y\n");
    file_.print("_atom_site_Cartn_z\n");


    const auto& positions = frame.positions();
    for (size_t i = 0; i < frame.size(); ++i) {
        const auto& atom = frame[i];

	// TODO: handle infinite cells
        Matrix3D invmat = frame.cell().matrix().invert();
	Vector3D fract = invmat * positions[i];

        file_.print("{} {} 1.0 {: <4} {} {: >3} {} {: >4} {:8.3f} {:8.3f} {:8.3f} {} {} {}\n",
                atom.name(), atom.type(), fract[0], fract[1], fract[2],
                positions[i][0], positions[i][1], positions[i][2]
        );
    }

    models_++;
}
