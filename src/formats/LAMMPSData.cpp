// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <unordered_set>

#include "chemfiles/formats/LAMMPSData.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

atom_style::atom_style(const std::string& name): name_(name) {
    if (name == "angle") {
        style_ = ANGLE;
        expected_ = 6;
    } else if (name == "atomic") {
        style_ = ATOMIC;
        expected_ = 5;
    } else if (name == "body") {
        style_ = BODY;
        expected_ = 6;
    } else if (name == "bond") {
        style_ = BOND;
        expected_ = 6;
    } else if (name == "charge") {
        style_ = CHARGE;
        expected_ = 6;
    } else if (name == "dipole") {
        style_ = DIPOLE;
        expected_ = 6;
    } else if (name == "dpd") {
        style_ = DPD;
        expected_ = 5;
    } else if (name == "electron") {
        style_ = ELECTRON;
        expected_ = 5;
    } else if (name == "ellipsoid") {
        style_ = ELLIPSOID;
        expected_ = 5;
    } else if (name == "full") {
        style_ = FULL;
        expected_ = 7;
    } else if (name == "line") {
        style_ = LINE;
        expected_ = 6;
    } else if (name == "meso") {
        style_ = MESO;
        expected_ = 5;
    } else if (name == "molecular") {
        style_ = MOLECULAR;
        expected_ = 6;
    } else if (name == "peri") {
        style_ = PERI;
        expected_ = 5;
    } else if (name == "smd") {
        style_ = SMD;
        expected_ = 7;
    } else if (name == "sphere") {
        style_ = SPHERE;
        expected_ = 5;
    } else if (name == "template") {
        style_ = TEMPLATE;
        expected_ = 6;
    } else if (name == "tri") {
        style_ = TRI;
        expected_ = 6;
    } else if (name == "wavepacket") {
        style_ = WAVEPACKET;
        expected_ = 6;
    } else if (name == "hybrid") {
        style_ = HYBRID;
        expected_ = 5;
    } else {
        throw FormatError("Unknown atom style " + name);
    }
    assert(expected_ != 0);
}

atom_data atom_style::read_line(const std::string& line) const {
    atom_data data;
    int assigned = 0;
    switch (style_) {
    case ANGLE:
    case BOND:
    case MOLECULAR:
        // atom-ID molecule-ID atom-type x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %zu %lf %lf %lf",
            &data.index, &data.molid, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case ATOMIC:
        // atom-ID atom-type x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %lf %lf %lf",
            &data.index, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case BODY:
        // atom-ID atom-type bodyflag mass x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %*d %lf %lf %lf %lf",
            &data.index, &data.type, &data.mass, &data.x, &data.y, &data.z
        );
        break;
    case CHARGE:
    case DIPOLE:
        // atom-ID atom-type q x y z
        // atom-ID atom-type q x y z mux muy muz
        assigned = sscanf(
            line.c_str(), "%zu %zu %lf %lf %lf %lf",
            &data.index, &data.type, &data.charge, &data.x, &data.y, &data.z
        );
        break;
    case DPD:
        // atom-ID atom-type theta x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %*f %lf %lf %lf",
            &data.index, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case ELECTRON:
        // atom-ID atom-type q spin eradius x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %*f %*f %lf %lf %lf",
            &data.index, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case ELLIPSOID:
        // atom-ID atom-type ellipsoidflag density x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %*d %*f %lf %lf %lf",
            &data.index, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case FULL:
        // atom-ID molecule-ID atom-type q x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %zu %lf %lf %lf %lf",
            &data.index, &data.molid, &data.type, &data.charge, &data.x, &data.y, &data.z
        );
        break;
    case LINE:
        // atom-ID molecule-ID atom-type lineflag density x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %zu %*d %*f %lf %lf %lf",
            &data.index, &data.molid, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case MESO:
        // atom-ID atom-type rho e cv x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %*f %*f %*f %lf %lf %lf",
            &data.index, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case PERI:
        // atom-ID atom-type volume density x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %*f %*f %lf %lf %lf",
            &data.index, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case SMD:
        // atom-ID atom-type molecule volume mass kernel-radius contact-radius x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %zu %*f %lf %*f %*f %lf %lf %lf",
            &data.index, &data.type, &data.molid, &data.mass, &data.x, &data.y, &data.z
        );
        break;
    case SPHERE:
        // atom-ID atom-type diameter density x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %*f %*f %lf %lf %lf",
            &data.index, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case TEMPLATE:
        // atom-ID molecule-ID template-index template-atom atom-type x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %*d %*d %zu %lf %lf %lf",
            &data.index, &data.molid, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case TRI:
        // atom-ID molecule-ID atom-type triangleflag density x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %zu %*d %*f %lf %lf %lf",
            &data.index, &data.molid, &data.type, &data.x, &data.y, &data.z
        );
        break;
    case WAVEPACKET:
        // atom-ID atom-type charge spin eradius etag cs_re cs_im x y z
        assigned = sscanf(
            line.c_str(), "%zu %zu %lf %*f %*f %*d %*f %*f %lf %lf %lf",
            &data.index, &data.type, &data.charge, &data.x, &data.y, &data.z
        );
        break;
    case HYBRID:
        if (!warned_) {
            warning("Only reading the first style for atom_style hybrid");
            warned_ = true;
        }
        // atom-ID atom-type x y z sub-style1 sub-style2 ...
        assigned = sscanf(
            line.c_str(), "%zu %zu %lf %lf %lf",
            &data.index, &data.type, &data.x, &data.y, &data.z
        );
        break;
    }

    if (assigned != expected_) {
        throw FormatError("invalid line for this atom style " + name_ + " '" + line + "'");
    }

    // LAMMPS uses 1-based indexing
    data.index -= 1;
    if (data.molid != static_cast<size_t>(-1)) {
        data.molid -= 1;
    }

    return data;
}

/// Split a string in data (stays in the `line` parameter) and comment
/// (in the return value).
static std::string split_comment(std::string& line);
/// Check if the line is an unused header value
static bool is_unused_header(const std::string& line);
/// Cast a long long value to a size_t, while checking that the value does fit
/// in a size_t
static size_t checked_cast(long long int value);

LAMMPSDataFormat::LAMMPSDataFormat(const std::string& path, File::Mode mode):
    current_section_(HEADER), file_(TextFile::create(path, mode)), style_("full") {}

size_t LAMMPSDataFormat::nsteps() {
    return 1;
}

void LAMMPSDataFormat::read_step(const size_t step, Frame& frame) {
    assert(step == 0);
    file_->rewind();
    current_section_ = HEADER;
    read(frame);
}

void LAMMPSDataFormat::read(Frame& frame) {
    auto comment = file_->readline();

    // VMD topotools writes the atom style in the comment header
    auto it = comment.find("atom_style");
    if (it != std::string::npos) {
        auto end = trim(comment.substr(it + 10));
        atom_style_name_ = trim(split(end, ' ')[0]);
    }

    while(!file_->eof()) {
        switch (current_section_) {
        case HEADER:
            read_header(frame);
            break;
        case ATOMS:
            read_atoms(frame);
            break;
        case MASSES:
            read_masses();
            break;
        case BONDS:
            read_bonds(frame);
            break;
        case VELOCITIES:
            read_velocities(frame);
            break;
        case IGNORED:
            skip_to_next_section();
            break;
        case NOT_A_SECTION:
            unreachable();
            break;
        }
    }

    setup_masses(frame);
    setup_names(frame);
}

void LAMMPSDataFormat::read_header(Frame& frame) {
    assert(current_section_ == HEADER);
    double a = 1.0;
    double b = 1.0;
    double c = 1.0;
    while (!file_->eof()) {
        auto line = file_->readline();
        auto content = line;
        split_comment(content);
        if (content.empty() || is_unused_header(content)) {
            // Nothing to do
        } else if (content.find("atoms") != std::string::npos) {
            natoms_ = read_header_integer(content, "atoms");
        } else if (content.find("bonds") != std::string::npos) {
            nbonds_ = read_header_integer(content, "bonds");
        } else if (content.find("atom types") != std::string::npos) {
            natom_types_ = read_header_integer(content, "atom types");
        } else if (content.find("xlo xhi") != std::string::npos) {
            a = read_header_box_bounds(content, "xlo xhi");
        } else if (content.find("ylo yhi") != std::string::npos) {
            b = read_header_box_bounds(content, "ylo yhi");
        } else if (content.find("zlo zhi") != std::string::npos) {
            c = read_header_box_bounds(content, "zlo zhi");
        } else if (content.find("xy xz yz") != std::string::npos) {
            throw FormatError(
                "Triclinic cells are not yet implemented with LAMMPS data format"
            );
        } else {
            // End of the header, get the section and break
            current_section_ = get_section(line);
            break;
        }
    }
    frame.set_cell(UnitCell(a, b, c));
}

size_t LAMMPSDataFormat::read_header_integer(const std::string& line, const std::string& context) {
    auto splitted = split(trim(line), ' ');
    if (splitted.size() < 2) {
        throw FormatError(
            "Invalid value: expected '<n> " + context + "', got '" + line + "'"
        );
    }
    return checked_cast(string2longlong(splitted[0]));
}

double LAMMPSDataFormat::read_header_box_bounds(const std::string& line, const std::string& context) {
    auto splitted = split(trim(line), ' ');
    if (splitted.size() < 4) {
        throw FormatError(
            "Invalid value: expected '<lo> <hi> " + context + "', got '" + line + "'"
        );
    }
    auto low = string2double(splitted[0]);
    auto high = string2double(splitted[1]);
    return high - low;
}

void LAMMPSDataFormat::get_next_section() {
    while (!file_->eof()) {
        auto line = file_->readline();
        if (!line.empty()) {
            auto section = get_section(line);
            if (section == NOT_A_SECTION) {
                throw FormatError("Expected section name, got " + line);
            } else {
                current_section_ = section;
                break;
            }
        }
    }
}

void LAMMPSDataFormat::skip_to_next_section() {
    while (!file_->eof()) {
        auto line = file_->readline();
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

void LAMMPSDataFormat::read_atoms(Frame& frame) {
    assert(current_section_ == ATOMS);
    if (natoms_ == 0) {
        throw FormatError("missing atoms count in header");
    }

    if (atom_style_name_ == "") {
        warning("Unknown LAMMPS atom style, defaulting to full.");
        warning("You can give the atom style like this: Atoms  # <style>");
        atom_style_name_ = "full";
    }
    style_ = atom_style(atom_style_name_);

    frame.resize(natoms_);
    auto positions = frame.positions();
    auto& topology = frame.topology();
    auto residues = std::vector<Residue>();

    size_t n = 0;
    while (n < natoms_ && !file_->eof()) {
        auto line = file_->readline();
        auto comment = split_comment(line);
        if (line.empty()) {continue;}

        auto data = style_.read_line(line);
        if (data.index > natoms_) {
            throw FormatError(
                "Too many atoms in Atoms section: expected " + std::to_string(natoms_) +
                " atoms, got atom at index " + std::to_string(data.index)
            );
        }

        if (!comment.empty()) {
            // Read the first string after the comment, and use it as atom name
            auto name = split(trim(comment), ' ')[0];
            if (names_.empty()) {
                names_.resize(natoms_);
            }
            names_[data.index] = name;
        }

        auto atom = Atom(std::to_string(data.type));
        if (!std::isnan(data.charge)) {
            atom.set_charge(data.charge);
        }
        if (!std::isnan(data.mass)) {
            atom.set_mass(data.mass);
        }

        if (data.molid != static_cast<size_t>(-1)) {
            residues.resize(data.molid + 1, Residue(""));
            residues[data.molid].add_atom(data.index);
        }

        topology[data.index] = atom;
        positions[data.index][0] = data.x;
        positions[data.index][1] = data.y;
        positions[data.index][2] = data.z;
        n++;
    }

    for (auto residue: residues) {
        topology.add_residue(std::move(residue));
    }

    get_next_section();
}

void LAMMPSDataFormat::read_masses() {
    assert(current_section_ == MASSES);
    if (natom_types_ == 0) {
        throw FormatError("missing atom types count in header");
    }
    size_t n = 0;
    while (n < natom_types_ && !file_->eof()) {
        auto line = file_->readline();
        split_comment(line);
        if (line.empty()) {continue;}

        auto splitted = split(trim(line), ' ');
        if (splitted.size() != 2) {
            throw FormatError("bad mass specification '" + line + "'");
        }

        auto type = splitted[0];
        auto mass = string2double(splitted[1]);
        masses_.emplace(std::move(type), mass);
        n++;
    }

    get_next_section();
}

void LAMMPSDataFormat::read_bonds(Frame& frame) {
    assert(current_section_ == BONDS);
    if (nbonds_ == 0) {
        throw FormatError("missing bonds count in header");
    }
    size_t n = 0;
    auto& topology = frame.topology();
    while (n < nbonds_ && !file_->eof()) {
        auto line = file_->readline();
        split_comment(line);
        if (line.empty()) {continue;}

        auto splitted = split(trim(line), ' ');
        if (splitted.size() != 4) {
            throw FormatError("bad bonds specification '" + line + "'");
        }
        // LAMMPS use 1-based indexing
        auto i = checked_cast(string2longlong(splitted[2]) - 1);
        auto j = checked_cast(string2longlong(splitted[3]) - 1);
        topology.add_bond(i, j);
        n++;
    }

    if (file_->eof() && n < nbonds_) {
        throw FormatError("end of file found before getting all bonds");
    }

    get_next_section();
}

void LAMMPSDataFormat::read_velocities(Frame& frame) {
    assert(current_section_ == VELOCITIES);
    if (natoms_ == 0) {
        throw FormatError("missing atoms count in header");
    }
    size_t n = 0;
    frame.add_velocities();
    auto velocities = *frame.velocities();
    while (n < natoms_ && !file_->eof()) {
        auto line = file_->readline();
        split_comment(line);
        if (line.empty()) {continue;}

        auto splitted = split(trim(line), ' ');
        if (splitted.size() < 4) {
            throw FormatError("bad velocity specification '" + line + "'");
        }
        // LAMMPS use 1-based indexing
        auto id = checked_cast(string2longlong(splitted[0]) - 1);
        auto vx = string2double(splitted[1]);
        auto vy = string2double(splitted[2]);
        auto vz = string2double(splitted[3]);
        velocities[id] = vector3d(vx, vy, vz);
        n++;
    }

    if (file_->eof() && n < nbonds_) {
        throw FormatError("end of file found before getting all velocities");
    }

    get_next_section();
}


void LAMMPSDataFormat::setup_masses(Frame& frame) const {
    if (masses_.empty()) {return;}
    for (auto& atom: frame.topology()) {
        auto it = masses_.find(atom.type());
        if (it != masses_.end()) {
            atom.set_mass(it->second);
        }
    }
}

void LAMMPSDataFormat::setup_names(Frame& frame) const {
    if (names_.empty()) {return;}
    assert(names_.size() == frame.natoms());

    auto& topology = frame.topology();
    for (size_t i=0; i<frame.natoms(); i++) {
        if (names_[i] != "") {


            topology[i].set_name(names_[i]);
            topology[i].set_type(names_[i]);
        }
    }
}


static std::unordered_set<std::string> IGNORED_SECTIONS = {
    "Ellipsoids", "Lines", "Triangles", "Bodies", "Pair Coeffs",
    "PairIJ Coeffs", "Bond Coeffs", "Angle Coeffs", "Dihedral Coeffs",
    "Improper Coeffs", "BondBond Coeffs", "BondAngle Coeffs",
    "MiddleBondTorsion Coeffs", "EndBondTorsion Coeffs", "AngleTorsion Coeffs",
    "AngleAngleTorsion Coeffs", "BondBond13 Coeffs", "AngleAngle Coeffs"
};

LAMMPSDataFormat::section_t LAMMPSDataFormat::get_section(std::string line) {
    auto comment = split_comment(line);
    auto section = trim(line);
    if (section == "Atoms") {
        if (!comment.empty()) {
            atom_style_name_ = trim(comment);
        }
        return ATOMS;
    } else if (section == "Bonds") {
        return BONDS;
    } else if (section == "Velocities") {
        return VELOCITIES;
    } else if (section == "Masses") {
        return MASSES;
    } else if (section == "Angles" || section == "Dihedrals" || section == "Impropers") {
        // We don't use the angles, dihedral and improper sections, but we don't
        // send a warning for them, as they are guessed from the bonds

        // TODO: check that the angles and dihedral list matches
        return IGNORED;
    } else if (IGNORED_SECTIONS.find(section) != IGNORED_SECTIONS.end()) {
        if (section.find("Coeffs") == std::string::npos) {
            // Don't send a warning for force field parameters
            warning("Ignored section '{}' in LAMMPS data file", section);
        }
        return IGNORED;
    } else {
        return NOT_A_SECTION;
    }
}

std::string split_comment(std::string& line) {
    std::string comment;
    auto position = line.find("#");
    if (position != std::string::npos) {
        comment = line.substr(position + 1);
        line.erase(position);
    }
    return comment;
}

bool is_unused_header(const std::string& line) {
    return (line.find("angles") != std::string::npos) ||
           (line.find("dihedrals") != std::string::npos) ||
           (line.find("impropers") != std::string::npos) ||
           (line.find("bond types") != std::string::npos) ||
           (line.find("angle types") != std::string::npos) ||
           (line.find("dihedral types") != std::string::npos) ||
           (line.find("improper types") != std::string::npos) ||
           (line.find("extra bond per atom") != std::string::npos) ||
           (line.find("extra angle per atom") != std::string::npos) ||
           (line.find("extra dihedral per atom") != std::string::npos) ||
           (line.find("extra improper per atom") != std::string::npos) ||
           (line.find("extra special per atom") != std::string::npos) ||
           (line.find("ellipsoids") != std::string::npos) ||
           (line.find("lines") != std::string::npos) ||
           (line.find("triangles") != std::string::npos) ||
           (line.find("bodies") != std::string::npos);
}


size_t checked_cast(long long int value) {
    if (value < 0) {
        throw FormatError("invalid integer: should be positive, is " + std::to_string(value));
    } else if (sizeof(long long) < sizeof(size_t) && value > static_cast<long long>(SIZE_MAX)) {
        throw FormatError(
            "invalid integer: " + std::to_string(value) + " is bigger than the available size_t"
        );
    } else {
        return static_cast<size_t>(value);
    }
}
