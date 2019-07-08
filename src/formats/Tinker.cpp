// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <istream>

#include <fmt/ostream.h>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Connectivity.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/sorted_set.hpp"

#include "chemfiles/formats/Tinker.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<TinkerFormat>() {
    return FormatInfo("Tinker").with_extension(".arc").description(
        "Tinker XYZ text format"
    );
}

static bool is_unit_cell_line(const std::string& line);

void TinkerFormat::read_next(Frame& frame) {
    size_t natoms = 0;
    try {
        auto line = file_->readline();
        scan(line, "%zu", &natoms);
    } catch (const FileError& e) {
        throw format_error(
            "can not read number of atoms in {}: {}", file_->path(), e.what()
        );
    }

    std::vector<std::string> lines;
    try {
        auto line = file_->readline();
        if (is_unit_cell_line(line)) {
            // Read the cell
            double a = 0, b = 0, c = 0;
            double alpha = 0, beta = 0, gamma = 0;
            scan(line, "%lf %lf %lf %lf %lf %lf", &a, &b, &c, &alpha, &beta, &gamma);
            frame.set_cell(UnitCell(a, b, c, alpha, beta, gamma));

            // And get the atoms lines
            lines = file_->readlines(natoms);
        } else {
            lines = file_->readlines(natoms - 1);
            lines.insert(lines.begin(), line);
        }

    } catch (const FileError& e) {
        throw format_error(
            "can not read atomic data in {}: {}", file_->path(), e.what()
        );
    }

    std::vector<std::vector<size_t>> bonds(natoms);
    frame.reserve(natoms);
    frame.resize(0);
    for (size_t i = 0; i < natoms; i++) {
        double x = 0, y = 0, z = 0;
        int id = 0, atom_type = 0;
        char name[32];
        int count = 0;
        scan(lines[i], "%d %31s %lf %lf %lf %d %n", &id, &name[0], &x, &y, &z, &atom_type, &count);

        frame.add_atom(Atom(name), Vector3D(x, y, z));
        while (static_cast<size_t>(count) != lines[i].size()) {
            size_t bonded = 0;
            int additional = 0;

            scan(lines[i].substr(static_cast<size_t>(count)), "%zu%n", &bonded, &additional);
            count += additional;

            bonds[i].push_back(bonded - 1);
        }
    }

    for (size_t i = 0; i < natoms; i++) {
        for (size_t j: bonds[i]) {
            frame.add_bond(i, j);
        }
    }
}

void TinkerFormat::write_next(const Frame& frame) {
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
        if (name.empty()) {
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
}

std::streampos TinkerFormat::forward() {
    if (!*file_) {
        return std::streampos(-1);
    }

    auto position = file_->tellg();
    size_t natoms = 0;
    try {
        auto line = file_->readline();
        if (trim(line).empty()) {
            // We just read an empty line, we give up here
            return std::streampos(-1);
        } else {
            // Get the number of atoms in the line
            natoms = parse<size_t>(split(trim(line), ' ')[0]);
        }
    } catch (const FileError&) {
        // No more line left in the file
        return std::streampos(-1);
    } catch (const Error&) {
        // We could not read an integer, so give up here
        return std::streampos(-1);
    }

    try {
        auto line = file_->readline();
        // Minus one because we just read a line.
        size_t lines_to_skip = natoms - 1;

        // This is how tinker does it to check if there is unit cell information
        // in the file, so let's follow them here.
        if (is_unit_cell_line(line)) {
            lines_to_skip += 1;
        }

        file_->skiplines(lines_to_skip);
    } catch (const FileError&) {
        // We could not read the lines from the file
        throw format_error(
            "not enough lines in '{}' for Tinker XYZ format", file_->path()
        );
    }
    return position;
}

bool is_unit_cell_line(const std::string& line) {
    static const char* LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return line.find_first_of(LETTERS) == std::string::npos;
}
