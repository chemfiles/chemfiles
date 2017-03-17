// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <cassert>
#include <sstream>

#include "fmt/format.h"
#include "fmt/ostream.h"

#include "chemfiles/formats/XYZ.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"

using namespace chemfiles;

XYZFormat::XYZFormat(const std::string& path, File::Mode mode)
    : file_(TextFile::create(path, mode)), step_cursor_(0) {}

std::string XYZFormat::description() const {
    return "XYZ file format.";
}

bool XYZFormat::forward(size_t nsteps) {
    size_t i = 0;
    // Move the file pointer to the good position step by step, as the number of
    // atoms may not be constant
    std::string line;
    while (i < nsteps) {
        try {
            line = file_->readline();
            auto natoms = std::stoul(line);
            file_->readlines(natoms + 1);
            step_cursor_++;
        } catch (const std::invalid_argument&) {
            // We could not read an integer, so give up here
            return false;
        } catch (const FileError&) {
            // We could not read the lines from the file
            throw FormatError("Not enough lines in '" + file_->filename() +
                              "' for XYZ format at step " + std::to_string(i));
        }
        i++;
    }
    return true;
}

size_t XYZFormat::nsteps() {
    file_->rewind();
    size_t n = 0;
    while (!file_->eof()) {
        if (forward(1)) {
            n++;
        }
    }
    file_->rewind();
    return n;
}

void XYZFormat::read_step(const size_t step, Frame& frame) {
    file_->rewind();
    forward(step);
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
        auto type = topology[i].type();
        if (type == "") {type = "X";}
        fmt::print(
            *file_, "{} {} {} {}\n",
            type, positions[i][0], positions[i][1], positions[i][2]
        );
    }
}
