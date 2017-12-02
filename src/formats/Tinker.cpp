// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/Tinker.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<TinkerFormat>() {
    return FormatInfo("Tinker").with_extension(".arc").description(
        "Tinker XYZ text format"
    );
}

/// Fast-forward the file for one step, returning `false` if the file does
/// not contain one more step.
static bool forward(TextFile& file);
static bool is_unit_cell_line(const std::string& line);

TinkerFormat::TinkerFormat(const std::string& path, File::Mode mode)
    : file_(TextFile::create(path, mode))
{
    while (!file_->eof()) {
        auto position = file_->tellg();
        if (!file_ || position == std::streampos(-1)) {
            throw format_error("IO error while reading '{}' as Tinker XYZ", path);
        }
        if (forward(*file_)) {
            steps_positions_.push_back(position);
        }
    }
    file_->rewind();
}

size_t TinkerFormat::nsteps() {
    return steps_positions_.size();
}

void TinkerFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

void TinkerFormat::read(Frame& frame) {
    size_t natoms = 0;
    try {
        // Get the number of atoms
        *file_ >> natoms;
        // and dismiss the rest of the line
        file_->readline();
    } catch (const FileError& e) {
        throw format_error(
            "can not read number of atoms in {}: {}", file_->filename(), e.what()
        );
    }

    std::vector<std::string> lines;
    try {
        auto line = file_->readline();
        if (is_unit_cell_line(line)) {
            auto splitted = split(line, ' ');

            // Read the cell
            if (splitted.size() != 6) {
                throw format_error("bad unit cell specification '{}'", line);
            }
            auto cell = std::vector<double>(6);
            std::transform(
                splitted.begin(), splitted.end(), cell.begin(), string2double
            );

            frame.set_cell(UnitCell(
                cell[0], cell[1], cell[2], cell[3], cell[4], cell[5]
            ));

            // And get the atoms lines
            lines = file_->readlines(natoms);
        } else {
            lines = file_->readlines(natoms - 1);
            lines.insert(lines.begin(), line);
        }

    } catch (const FileError& e) {
        throw format_error(
            "can not read atomic data in {}: {}", file_->filename(), e.what()
        );
    }

    std::vector<std::vector<size_t>> bonds(natoms);
    frame.reserve(natoms);
    frame.resize(0);
    for (size_t i = 0; i < natoms; i++) {
        std::istringstream string_stream;
        double x = 0, y = 0, z = 0;
        int id = 0, atom_type = 0;
        std::string name;

        string_stream.str(lines[i]);
        string_stream >> id >> name >> x >> y >> z >> atom_type;

        frame.add_atom(Atom(name), Vector3D(x, y, z));
        while (string_stream) {
            size_t bonded = 0;
            string_stream >> bonded;
            if (string_stream) {
                bonds[i].push_back(bonded - 1);
            }
        }
    }

    for (size_t i = 0; i < natoms; i++) {
        for (size_t j: bonds[i]) {
            frame.add_bond(i, j);
        }
    }
}

void TinkerFormat::write(const Frame& frame) {
    fmt::print(*file_, "{} written by the chemfiles library\n", frame.size());
    fmt::print(*file_, "{} {} {} {} {} {}\n",
        frame.cell().a(), frame.cell().b(), frame.cell().c(),
        frame.cell().alpha(), frame.cell().beta(), frame.cell().gamma()
    );

    auto& topology = frame.topology();
    // Build type index numbering. type_id will be searched for each atom type,
    // and the position can be used a an unique integer identifier for the atom
    // type.
    auto types_id = sorted_set<std::string>();
    for (auto& atom: topology) {
        types_id.insert(atom.type());
    }

    // Build bonds index. It will contains all atoms bonded to the atom i in
    // bonded_to[i].
    auto bonded_to = std::vector<std::vector<size_t>>(frame.size());
    for (auto& bond: topology.bonds()) {
        bonded_to[bond[0]].push_back(bond[1]);
        bonded_to[bond[1]].push_back(bond[0]);
    }

    auto& positions = frame.positions();
    for (size_t i = 0; i < frame.size(); i++) {
        auto name = topology[i].name();
        if (name == "") {
            name = "X";
        }
        auto it = types_id.find(topology[i].type());
        assert(it != types_id.end());
        auto type = (it - types_id.begin()) + 1;

        fmt::print(
            *file_, "{} {} {} {} {} {}",
            i + 1, name, positions[i][0], positions[i][1], positions[i][2], type
        );
        for (size_t other: bonded_to[i]) {
            fmt::print(*file_, " {}", other + 1);
        }
        fmt::print(*file_, "\n");
    }

    steps_positions_.push_back(file_->tellg());
}

bool forward(TextFile& file) {
    if (!file) {return false;}

    long long natoms = 0;
    try {
        auto line = file.readline();
        if (trim(line) == "") {
            // We just read an empty line, we give up here
            return false;
        } else {
            // Get the number of atoms in the line
            natoms = std::stoll(split(trim(line), ' ')[0]);
        }
    } catch (const FileError&) {
        // No more line left in the file
        return false;
    } catch (const std::invalid_argument&) {
        // We could not read an integer, so give up here
        return false;
    }

    if (natoms < 0) {
        throw format_error(
            "number of atoms can not be negative in '{}'", file.filename()
        );
    }

    try {
        auto line = file.readline();
        // Minus one because we just read a line.
        size_t lines_to_skip = static_cast<size_t>(natoms) - 1;

        // This is how tinker does it to check if there is unit cell information
        // in the file, so let's follow them here.
        if (is_unit_cell_line(line)) {
            lines_to_skip += 1;
        }

        file.readlines(lines_to_skip);
    } catch (const FileError&) {
        // We could not read the lines from the file
        throw format_error(
            "not enough lines in '{}' for Tinker XYZ format", file.filename()
        );
    }
    return true;
}

bool is_unit_cell_line(const std::string& line) {
    static const char* LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return line.find_first_of(LETTERS) == std::string::npos;
}
