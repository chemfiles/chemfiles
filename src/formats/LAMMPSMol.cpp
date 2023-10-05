// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <cstdint>

#include <map>
#include <set>
#include <array>
#include <string>
#include <vector>
#include <exception>
#include <unordered_map>

#include <fmt/format.h>

#include "chemfiles/types.hpp"
#include "chemfiles/parse.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/string_view.hpp"
#include "chemfiles/unreachable.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/File.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/FormatMetadata.hpp"

#include "chemfiles/formats/LAMMPSMol.hpp"

using namespace chemfiles;

template<> const FormatMetadata& chemfiles::format_metadata<LAMMPSMolFormat>() {
    static FormatMetadata metadata;
    metadata.name = "LAMMPSMol";
    metadata.extension = ".mol";
    metadata.description = "LAMMPSMol molecule template";
    metadata.reference = "http://lammps.sandia.gov/doc/molecule.html";

    metadata.read = true;
    metadata.write = true;
    metadata.memory = false;

    metadata.positions = true;
    metadata.velocities = true;
    metadata.unit_cell = false;
    metadata.atoms = true;
    metadata.bonds = true;
    metadata.residues = false;
    return metadata;
}

void LAMMPSMolFormat::read_next(Frame& frame) {
    if (file_.tellpos() != 0) {
        throw format_error("LAMMPSMol format can only contain one frame");
    }

    auto comment = file_.readline();
    while (!file_.eof()) {
        switch (current_section_) {
            case HEADER:
                read_header(frame);
                break;
            case COORDS:
                read_coords(frame);
                break;
            case MASSES:
                read_masses();
                break;
            case BONDS:
                read_bonds(frame);
                break;
            case ANGLES:
                read_angles(frame);
                break;
            case DIHEDRALS:
                read_dihedrals(frame);
                break;
            case IMPROPERS:
                read_impropers(frame);
                break;
            case TYPES:
                read_types(frame);
                break;

        }
    }
    setup_masses(frame);
    setup_names(frame);
}

/// Remove the comment from `line` and return it.
static string_view split_comment(string_view& line);
/// Check if the line is an unused header value
static bool is_unused_header(string_view line);

string_view split_comment(string_view& line) {
    auto position = line.find('#');
    if (position != std::string::npos) {
        auto comment = line.substr(position + 1);
        line.remove_suffix(line.size() - position);
        return comment;
    } else {
        return "";
    }
}

bool is_unused_header(string_view line) {
    return (line.find("atom types") != std::string::npos) ||
           (line.find("bond types") != std::string::npos) ||
              (line.find("angle types") != std::string::npos) ||
                (line.find("dihedral types") != std::string::npos);
}

void LAMMPSMolFormat::read_header(Frame& frame) {
    assert(current_section_ == HEADER);
    
    while (!file_.eof()) {
        auto line = file_.readline();
        auto content = line;
        split_comment(content);
        if (content.empty() || is_unused_header(content)) {
            // Nothing to do
        } else if (content.find("atoms") != std::string::npos) {
            natoms_ = read_header_integer(content, "atoms");
        } else if (content.find("bonds") != std::string::npos) {
            nbonds_ = read_header_integer(content, "bonds");
        } // else if angles
        else {
            // End of the header, get the section and break
            current_section_ = get_section(line);
            assert (current_section_ != NOT_A_SECTION);
            break;
        }
    }
}

size_t LAMMPSDataFormat::read_header_integer(string_view line, const std::string& context) {
    auto splitted = split(line, ' ');
    if (splitted.size() < 2) {
        throw format_error(
            "invalid header value: expected '<n> {}', got '{}'", context, line
        );
    }
    return parse<size_t>(splitted[0]);
}

static std::unordered_set<string_view> IGNORED_SECTIONS = {
    "Fragments", "Diameters", "Special Bond Count", "Special Bonds", "Shake Flags", "Shake Atoms", "Shake Bond Types"
};

LAMMPSMolFormat::section_t LAMMPSDataFormat::get_section(string_view line) {
    auto comment = split_comment(line);
    auto section = trim(line);
    if (section == "Coords") {
        return COORDS;
    } else if (section == "Bonds") {
        return BONDS;
    } else if (section == "Types") {
        return VELOCITIES;
    } else if (section == "Masses") {
        return MASSES;
    } else if (section == "Charges") {
        return CHARGES;
    } else if (IGNORED_SECTIONS.find(section) != IGNORED_SECTION.end()) {
        return IGNORED;
    }
    else {
        return NOT_A_SECTION;
    }
}

void LAMMPSMolFormat::read_coords(Frame& frame) {
    assert(current_section_ == COORDS);
    if (natoms_ == 0) {
        throw format_error("missing atoms count in header");
    }

    frame.resize(natoms_);
    auto positions = frame.positions();
    auto residues = std::unordered_map<size_t, Residue>();

    size_t n = 0;
    while (n < natoms_ && !file._eof()) {
        auto line = file_.readline();
        auto comment = split_comment(line);
        if (line.empty()) {continue;}

        auto splitted = split(line, ' ');
        auto index = parse<size_t>(splitted[0]);
        auto x = parse<double>(splitted[1]);
        auto y = parse<double>(splitted[2]);
        auto z = parse<double>(splitted[3]);
        frame[index] = Atom("");
        positions[index][0] = x;
        positions[index][1] = y;
        positions[index][2] = z;

        if (!comment.empty()) {
        // Read the first string after the comment, and use it as atom name
        auto name = split(comment, ' ')[0];
        if (names_.empty()) {
            names_.resize(natoms_);
        }
        names_[data.index] = name.to_string();
        }

        n++;
    }

    get_next_section();
}

void LAMMPSMolFormat::read_masses() {
    assert(current_section_ == MASSES);
    if (natom_types_ == 0) {
        throw format_error("missing atom types count in header");
    }
    size_t n = 0;
    auto line = file_.readline();
    split_comment(line);
    while (line.size() != 0 && !file_.eof()) {
        auto line = file_.readline();
        split_comment(line);
        if (line.empty()) {continue;}

        auto splitted = split(line, ' ');
        if (splitted.size() != 2) {
            throw format_error("bad mass specification '{}'", line);
        }

        auto type = splitted[0];
        auto mass = parse<double>(splitted[1]);
        masses_.emplace(type.to_string(), mass);
        n++;
    }

    get_next_section();
}

void LAMMPSMolFormat::read_types() {
    assert(current_section_ == TYPES);
    if (natom_types_ == 0) {
        throw format_error("missing atom types count in header");
    }
    size_t n = 0;
    while (n < natom_types_ && !file_.eof()) {
        auto line = file_.readline();
        split_comment(line);
        if (line.empty()) {continue;}

        auto splitted = split(line, ' ');
        if (splitted.size() != 2) {
            throw format_error("bad mass specification '{}'", line);
        }

        auto type = splitted[0];
        auto mass = parse<double>(splitted[1]);
        masses_.emplace(type.to_string(), mass);
        n++;
    }

    get_next_section();
}

void LAMMPSMolFormat::get_next_section() {
    while (!file_.eof()) {
        auto line = file_.readline();
        if (!line.empty()) {
            auto section = get_section(line);
            if (section == NOT_A_SECTION) {
                throw format_error("expected section name, got '{}'", line);
            } else {
                current_section_ = section;
                break;
            }
        }
    }
}

void LAMMPSDataFormat::skip_to_next_section() {
    while (!file_.eof()) {
        auto line = file_.readline();
        if (!line.empty()) {
            auto section = get_section(line);
            if (section == NOT_A_SECTION) {
                continue;
            } else {
                current_section_ = section;
                break;
            }
        }
    }
}
