// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <cmath>
#include <cstdint>

#include <array>
#include <string>
#include <vector>

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/optional.hpp"
#include "chemfiles/parse.hpp"
#include "chemfiles/string_view.hpp"
#include "chemfiles/types.hpp"
#include "chemfiles/utils.hpp"

#include "chemfiles/File.hpp"
#include "chemfiles/FormatMetadata.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/UnitCell.hpp"

#include "chemfiles/formats/LAMMPSAtom.hpp"

using namespace chemfiles;

template <> const FormatMetadata& chemfiles::format_metadata<LAMMPSAtomFormat>() {
    static FormatMetadata metadata;
    metadata.name = "LAMMPS";
    metadata.extension = ".lammpstrj";
    metadata.description = "LAMMPS text trajectory format";
    metadata.reference = "https://lammps.sandia.gov/doc/dump.html";

    metadata.read = true;
    metadata.write = false;
    metadata.memory = true;

    metadata.positions = true;
    metadata.velocities = true;
    metadata.unit_cell = true;
    metadata.atoms = true;
    metadata.bonds = false;
    metadata.residues = false;
    return metadata;
}

using chemfiles::private_details::is_upper_triangular;

static optional<string_view> get_item(string_view line) {
    auto splitted = split(line, ':');
    if (splitted.size() != 2 || trim(splitted[0]) != "ITEM") {
        return nullopt;
    }
    return trim(splitted[1]);
}

std::array<double, 3> LAMMPSAtomFormat::read_cell(Frame& frame) {
    auto line = file_.readline();
    auto item = get_item(line);
    try {
        if (!item) {
            throw format_error("expected an ITEM entry in LAMMPS format, got '{}'", line);
        }
        auto splitted = split(*item, ' ');
        if (splitted[0] == "BOX" && splitted[1] == "BOUNDS") {
            auto matrix = Matrix3D::unit();
            std::array<double, 3> origin;
            auto shape = UnitCell::ORTHORHOMBIC;
            if (splitted.size() >= 5 && (*item).find("xy xz yz") != string_view::npos) {
                shape = UnitCell::TRICLINIC;
            }
            line = file_.readline();
            auto splitted = split(line, ' ');
            if ((shape == UnitCell::ORTHORHOMBIC && splitted.size() != 2) ||
                (shape == UnitCell::TRICLINIC && splitted.size() != 3)) {
                size_t expected_dims = (shape == UnitCell::ORTHORHOMBIC) ? 2 : 3;
                throw format_error(
                    "incomplete box dimensions in LAMMPS format, expected {} but got {}",
                    expected_dims, splitted.size());
            }
            double xlo = parse<double>(splitted[0]);
            double xhi = parse<double>(splitted[1]);
            matrix[0][0] = xhi - xlo;
            origin[0] = xlo;
            if (shape == UnitCell::TRICLINIC) {
                matrix[0][1] = parse<double>(splitted[2]);
            }

            line = file_.readline();
            splitted = split(line, ' ');
            if ((shape == UnitCell::ORTHORHOMBIC && splitted.size() != 2) ||
                (shape == UnitCell::TRICLINIC && splitted.size() != 3)) {
                size_t expected_dims = (shape == UnitCell::ORTHORHOMBIC) ? 2 : 3;
                throw format_error(
                    "incomplete box dimensions in LAMMPS format, expected {} but got {}",
                    expected_dims, splitted.size());
            }
            double ylo = parse<double>(splitted[0]);
            double yhi = parse<double>(splitted[1]);
            matrix[1][1] = yhi - ylo;
            origin[1] = ylo;
            if (shape == UnitCell::TRICLINIC) {
                matrix[0][2] = parse<double>(splitted[2]);
            }

            line = file_.readline();
            splitted = split(line, ' ');
            if ((shape == UnitCell::ORTHORHOMBIC && splitted.size() != 2) ||
                (shape == UnitCell::TRICLINIC && splitted.size() != 3)) {
                size_t expected_dims = (shape == UnitCell::ORTHORHOMBIC) ? 2 : 3;
                throw format_error(
                    "incomplete box dimensions in LAMMPS format, expected {} but got {}",
                    expected_dims, splitted.size());
            }
            double zlo = parse<double>(splitted[0]);
            double zhi = parse<double>(splitted[1]);
            matrix[2][2] = zhi - zlo;
            origin[2] = zlo;
            if (shape == UnitCell::TRICLINIC) {
                matrix[1][2] = parse<double>(splitted[2]);
            }

            auto cell = UnitCell(matrix);
            frame.set_cell(cell);
            return origin;
        } else {
            throw format_error("missing 'BOX BOUNDS' item in LAMMPS format");
        }
    } catch (const Error& e) {
        throw format_error("can not read box header in LAMMPS format: {}", e.what());
    }
}

// LAMMPS is able to dump various per-atom properties and arbitrary user-defined variables
// posible per-atom attributes by dump command
enum lammps_atom_attr_t {
    // other possible attributes that are not important for chemfiles
    UNKNOWN,
    // atom ID
    ATOMID,
    // atom type
    TYPE,
    // name of atom element
    ELEMENT,
    // atom mass
    MASS,
    // unscaled atom coordinates
    POSX,
    POSY,
    POSZ,
    // scaled atom coordinates
    POSXS,
    POSYS,
    POSZS,
    // unwrapped atom coordinates
    POSXU,
    POSYU,
    POSZU,
    // scaled unwrapped atom coordinates
    POSXSU,
    POSYSU,
    POSZSU,
    // box image that the atom is in
    IMGX,
    IMGY,
    IMGZ,
    // atom velocities
    VELX,
    VELY,
    VELZ,
    // atom charge
    CHARGE,
};

// LAMMPS is able to dump the atomic positions in multiple formats
// multiple of these representations might be present simultaneously in a frame
// possible representations of the atomic positions
enum lammps_position_representation_t {
    // no atomic positions in frame
    NOPOS,
    // wrapped positions as x,y,z
    WRAPPED,
    // scaled positions as xs,ys,zs
    SCALED,
    // unwrapped positions as xu,yu,zu
    UNWRAPPED,
    // scaled unwrapped positions as xsu,ysu,zsu
    SCALED_UNWRAPPED,
};

static lammps_atom_attr_t attribute_from_str(string_view attr_str) {
    if (attr_str == "id") {
        return ATOMID;
    } else if (attr_str == "type") {
        return TYPE;
    } else if (attr_str == "element") {
        return ELEMENT;
    } else if (attr_str == "mass") {
        return MASS;
    } else if (attr_str == "x") {
        return POSX;
    } else if (attr_str == "y") {
        return POSY;
    } else if (attr_str == "z") {
        return POSZ;
    } else if (attr_str == "xs") {
        return POSXS;
    } else if (attr_str == "ys") {
        return POSYS;
    } else if (attr_str == "zs") {
        return POSZS;
    } else if (attr_str == "xu") {
        return POSXU;
    } else if (attr_str == "yu") {
        return POSYU;
    } else if (attr_str == "zu") {
        return POSZU;
    } else if (attr_str == "xsu") {
        return POSXSU;
    } else if (attr_str == "ysu") {
        return POSYSU;
    } else if (attr_str == "zsu") {
        return POSZSU;
    } else if (attr_str == "ix") {
        return IMGX;
    } else if (attr_str == "iy") {
        return IMGY;
    } else if (attr_str == "iz") {
        return IMGZ;
    } else if (attr_str == "vx") {
        return VELX;
    } else if (attr_str == "vy") {
        return VELY;
    } else if (attr_str == "vz") {
        return VELZ;
    } else if (attr_str == "q") {
        return CHARGE;
    } else {
        return UNKNOWN;
    }
}

static lammps_position_representation_t
detect_best_pos_representation(const std::vector<lammps_atom_attr_t>& fields) {
    int wrapped_count = 0;
    int scaled_count = 0;
    int unwrapped_count = 0;
    int scaled_unwrapped_count = 0;
    for (const auto& attr : fields) {
        switch (attr) {
        case POSX:
        case POSY:
        case POSZ:
            ++wrapped_count;
            break;
        case POSXS:
        case POSYS:
        case POSZS:
            ++scaled_count;
            break;
        case POSXU:
        case POSYU:
        case POSZU:
            ++unwrapped_count;
            break;
        case POSXSU:
        case POSYSU:
        case POSZSU:
            ++scaled_unwrapped_count;
            break;
        default:
            break;
        }
    }
    // choose the coordinate style that is defined for the most dimensions
    // prefer unwrapped coordinates over wrapped coordinates
    // and prefer non-scaled coordinates over scaled coordinates
    if (wrapped_count == 0 && scaled_count == 0 && unwrapped_count == 0 &&
        scaled_unwrapped_count == 0) {
        // no positions defined
        return NOPOS;
    } else if (unwrapped_count >= scaled_unwrapped_count && unwrapped_count >= wrapped_count &&
               unwrapped_count >= scaled_count) {
        return UNWRAPPED;
    } else if (scaled_unwrapped_count >= wrapped_count && scaled_unwrapped_count >= scaled_count) {
        return SCALED_UNWRAPPED;
    } else if (wrapped_count >= scaled_count) {
        return WRAPPED;
    } else {
        return SCALED;
    }
}

static void unwrap(Vector3D& position, std::array<int, 3>& image, Matrix3D& matrix) {
    // unwrap coordinates by using image data
    position[0] += image[0] * matrix[0][0] + image[1] * matrix[0][1] + image[2] * matrix[0][2];
    position[1] += image[1] * matrix[1][1] + image[2] * matrix[1][2];
    position[2] += image[2] * matrix[2][2];
}

void LAMMPSAtomFormat::read_next(Frame& frame) {
    auto item = get_item(file_.readline());
    if (!item) {
        throw format_error("can not read next step as LAMMPS format: expected an ITEM entry");
    }
    if (*item == "UNITS") { // optional
        frame.set("lammps_units", trim(file_.readline()).to_string());
        item = get_item(file_.readline());
        if (!item) {
            throw format_error("can not read next step as LAMMPS format: expected an ITEM entry");
        }
    }
    if (*item == "TIME") { // optional
        double time = parse<double>(trim(file_.readline()));
        frame.set("time", time);
        item = get_item(file_.readline());
        if (!item) {
            throw format_error("can not read next step as LAMMPS format: expected an ITEM entry");
        }
    }

    if (*item == "TIMESTEP") {
        int64_t timestep = parse<int64_t>(trim(file_.readline()));
        frame.set_step(static_cast<size_t>(timestep));
    } else {
        throw format_error("can not read next step as LAMMPS format: expected 'TIMESTEP' got '{}'",
                           *item);
    }

    size_t natoms = 0;
    item = get_item(file_.readline());
    if (item && *item == "NUMBER OF ATOMS") {
        int64_t lmp_natoms = parse<int64_t>(trim(file_.readline()));
        natoms = static_cast<size_t>(lmp_natoms);
    } else {
        throw format_error("can not read next step as LAMMPS format: expected "
                           "'NUMBER OF ATOMS' got '{}'",
                           *item);
    }

    // LAMMPS can have boxes that do not use (0,0,0) as origin
    auto origin = read_cell(frame);

    item = get_item(file_.readline());
    if (!item) {
        throw format_error("can not read next step as LAMMPS format: expected an ITEM entry");
    }
    auto splitted = split(*item, ' ');
    if (splitted.empty() || splitted[0] != "ATOMS") {
        throw format_error("can not read next step as LAMMPS format: expected 'ATOMS' got '{}'",
                           *item);
    }
    std::vector<lammps_atom_attr_t> fields;
    fields.reserve(splitted.size() - 1);
    optional<size_t> atomid_column = nullopt;
    std::vector<bool> duplicate_check;
    optional<std::vector<std::array<int, 3>>> images = nullopt;
    for (size_t i = 1; i < splitted.size(); ++i) {
        auto attr = attribute_from_str(splitted[i]);
        if (attr == ATOMID) {
            atomid_column = i - 1;
            duplicate_check = std::vector<bool>(natoms, false);
        }
        if (attr == VELX || attr == VELY || attr == VELZ) {
            frame.add_velocities();
        }
        if (attr == IMGX || attr == IMGY || attr == IMGZ) {
            images = std::vector<std::array<int, 3>>(natoms, {0, 0, 0});
        }
        fields.push_back(attr);
    }
    lammps_position_representation_t use_pos_repr = detect_best_pos_representation(fields);

    frame.resize(natoms);
    auto positions = frame.positions();
    auto velocities = frame.velocities();

    for (size_t i = 0; i < natoms; ++i) {
        auto line = file_.readline();
        auto splitted = split(line, ' ');
        if (splitted.size() != fields.size()) {
            throw format_error("LAMMPS line has wrong number of fields: expected {} got {}",
                               fields.size(), splitted.size());
        }

        size_t atomid = i;
        if (atomid_column) {
            // LAMMPS uses atom IDs that start with 1
            atomid = parse<size_t>(splitted[*atomid_column]);
            assert(atomid > 0);
            --atomid; // the frame uses zero-based indices
            assert(duplicate_check.size() == natoms);
            if (duplicate_check[atomid]) {
                throw format_error(
                    "found atoms with the same ID in LAMMPS format: {} is already present",
                    atomid + 1);
            }
            duplicate_check[atomid] = true;
        }

        auto& atom = frame[atomid];
        for (size_t j = 0; j < fields.size(); ++j) {
            switch (fields[j]) {
            case TYPE:
                atom.set_type(splitted[j].to_string());
                break;
            case ELEMENT:
                atom.set_name(splitted[j].to_string());
                break;
            case MASS:
                atom.set_mass(parse<double>(splitted[j]));
                break;
            case POSX:
                if (use_pos_repr == WRAPPED) {
                    positions[atomid][0] = parse<double>(splitted[j]);
                }
                break;
            case POSY:
                if (use_pos_repr == WRAPPED) {
                    positions[atomid][1] = parse<double>(splitted[j]);
                }
                break;
            case POSZ:
                if (use_pos_repr == WRAPPED) {
                    positions[atomid][2] = parse<double>(splitted[j]);
                }
                break;
            case POSXS:
                if (use_pos_repr == SCALED) {
                    // store scaled position (same for POSYS and POSZS)
                    // transform at the end when all three coordinates are known
                    positions[atomid][0] = parse<double>(splitted[j]);
                }
                break;
            case POSYS:
                if (use_pos_repr == SCALED) {
                    positions[atomid][1] = parse<double>(splitted[j]);
                }
                break;
            case POSZS:
                if (use_pos_repr == SCALED) {
                    positions[atomid][2] = parse<double>(splitted[j]);
                }
                break;
            case POSXU:
                if (use_pos_repr == UNWRAPPED) {
                    positions[atomid][0] = parse<double>(splitted[j]);
                }
                break;
            case POSYU:
                if (use_pos_repr == UNWRAPPED) {
                    positions[atomid][1] = parse<double>(splitted[j]);
                }
                break;
            case POSZU:
                if (use_pos_repr == UNWRAPPED) {
                    positions[atomid][2] = parse<double>(splitted[j]);
                }
                break;
            case POSXSU:
                if (use_pos_repr == SCALED_UNWRAPPED) {
                    // store scaled position (same for POSYSU and POSZSU)
                    // transform at the end when all three coordinates are known
                    positions[atomid][0] = parse<double>(splitted[j]);
                }
                break;
            case POSYSU:
                if (use_pos_repr == SCALED_UNWRAPPED) {
                    positions[atomid][1] = parse<double>(splitted[j]);
                }
                break;
            case POSZSU:
                if (use_pos_repr == SCALED_UNWRAPPED) {
                    positions[atomid][2] = parse<double>(splitted[j]);
                }
                break;
            case IMGX:
                assert(images);
                (*images)[atomid][0] = parse<int>(splitted[j]);
                break;
            case IMGY:
                assert(images);
                (*images)[atomid][1] = parse<int>(splitted[j]);
                break;
            case IMGZ:
                assert(images);
                (*images)[atomid][2] = parse<int>(splitted[j]);
                break;
            case VELX:
                assert(velocities);
                (*velocities)[atomid][0] = parse<double>(splitted[j]);
                break;
            case VELY:
                assert(velocities);
                (*velocities)[atomid][1] = parse<double>(splitted[j]);
                break;
            case VELZ:
                assert(velocities);
                (*velocities)[atomid][2] = parse<double>(splitted[j]);
                break;
            case CHARGE: {
                double charge = parse<double>(splitted[j]);
                atom.set_charge(charge);
            } break;
            case ATOMID:
            case UNKNOWN:
                break;
            }
        }
    }

    if (use_pos_repr == SCALED || use_pos_repr == SCALED_UNWRAPPED) {
        // all atoms currently know their scales position
        // transform the scaled coordinates to a non-scaled representation
        auto matrix = frame.cell().matrix();
        for (size_t i = 0; i < natoms; ++i) {
            // x = xlo + xs * (xhi - xlo) + ys * xy + zs * xz
            positions[i][0] = origin[0] + positions[i][0] * matrix[0][0] +
                              positions[i][1] * matrix[0][1] + positions[i][2] * matrix[0][2];
            // y = ylo + ys * (yhi - ylo) + z * yz
            positions[i][1] =
                origin[1] + positions[i][1] * matrix[1][1] + positions[i][2] * matrix[1][2];
            // z = zlo + zs * (zhi - zlo)
            positions[i][2] = origin[2] + positions[i][2] * matrix[2][2];
            if (images && use_pos_repr != SCALED_UNWRAPPED) {
                // unwrap coordinates by using image data
                unwrap(positions[i], (*images)[i], matrix);
            }
        }
    } else if (images && use_pos_repr != UNWRAPPED) {
        // unwrap coordinates by using image data
        auto matrix = frame.cell().matrix();
        for (size_t i = 0; i < natoms; ++i) {
            unwrap(positions[i], (*images)[i], matrix);
        }
    }
}

optional<uint64_t> LAMMPSAtomFormat::forward() {
    auto position = file_.tellpos();
    size_t natoms = 0;
    try {
        auto line = file_.readline();
        if (line.empty() && file_.eof()) {
            // no more data to read, so give up here
            return nullopt;
        }
        auto item = get_item(line);
        while (!file_.eof() && (!item || *item != "NUMBER OF ATOMS")) {
            line = file_.readline();
            item = get_item(line);
        }
        if (!item || *item != "NUMBER OF ATOMS") {
            throw format_error("could not find the number of atoms header in LAMMPS format");
        }
        natoms = parse<size_t>(trim(file_.readline()));
        // read the box
        for (size_t i = 0; i < 4; ++i) {
            file_.readline();
            if (file_.eof()) {
                throw format_error("could not read box data in LAMMPS format");
            }
        }
        file_.readline();
        if (file_.eof()) {
            throw format_error("could not read atom header in LAMMPS format");
        }

        for (size_t i = 0; i < natoms; ++i) {
            file_.readline();
            if (file_.eof()) {
                throw format_error("could not read enough atoms in LAMMPS format");
            }
        }
    } catch (const Error& e) {
        throw format_error("not enough lines in '{}' for LAMMPS format: {}", file_.path(),
                           e.what());
    }

    return position;
}
