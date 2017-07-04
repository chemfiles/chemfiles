// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <sstream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/XYZ.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"

using namespace chemfiles;

/// Fast-forward the file for one step, returning `false` if the file does
/// not contain one more step.
static bool forward(TextFile& file);

XYZFormat::XYZFormat(const std::string& path, File::Mode mode): file_(TextFile::create(path, mode)), steps_positions_() {
    while (!file_->eof()) {
        auto position = file_->tellg();
        if (!file_ || position == std::streampos(-1)) {
            throw FormatError("Error while reading '" + path + "' as XYZ");
        }
        if (forward(*file_)) {
            steps_positions_.push_back(position);
        }
    }
    file_->rewind();
}

size_t XYZFormat::nsteps() {
    return steps_positions_.size();
}

void XYZFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

void XYZFormat::read(Frame& frame) {
    size_t natoms = 0;
    try {
        natoms = std::stoul(file_->readline());
        file_->readline(); // XYZ comment line;
    } catch (const std::exception& e) {
        throw FormatError("Can not read next step: " + std::string(e.what()));
    }

    std::vector<std::string> lines;
    try {
        lines = file_->readlines(natoms);
    } catch (const FileError& e) {
        throw FormatError("Can not read file: " + std::string(e.what()));
    }

    frame.reserve(natoms);
    frame.resize(0);

    for (size_t i = 0; i < lines.size(); i++) {
        std::istringstream string_stream;
        double x = 0, y = 0, z = 0;
        std::string name;

        string_stream.str(lines[i]);
        string_stream >> name >> x >> y >> z;

        frame.add_atom(Atom(name), {{x, y, z}});
    }
}

void XYZFormat::write(const Frame& frame) {
    auto& topology = frame.topology();
    auto& positions = frame.positions();
    assert(frame.natoms() == topology.natoms());

    fmt::print(*file_, "{}\n", frame.natoms());
    fmt::print(*file_, "Written by the chemfiles library\n", frame.natoms());

    for (size_t i = 0; i < frame.natoms(); i++) {
        auto name = topology[i].name();
        if (name == "") {name = "X";}
        fmt::print(
            *file_, "{} {} {} {}\n",
            name, positions[i][0], positions[i][1], positions[i][2]
        );
    }

    steps_positions_.push_back(file_->tellg());
}

bool forward(TextFile& file) {
    if (!file) {return false;}

    std::string line;
    try {
        line = file.readline();
    } catch (const FileError&) {
        // No more line left in the file
        return false;
    }

    long long natoms = 0;
    try {
        natoms = std::stoll(line);
    } catch (const std::invalid_argument&) {
        // We could not read an integer, so give up here
        return false;
    }

    if (natoms < 0) {
        throw FormatError(
            "Number of atoms can not be negative in '" + file.filename()
        );
    }

    try {
        file.readlines(static_cast<size_t>(natoms) + 1);
    } catch (const FileError&) {
        // We could not read the lines from the file
        throw FormatError(
            "Not enough lines in '" + file.filename() + "' for XYZ format"
        );
    }
    return true;
}
