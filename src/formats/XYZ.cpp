/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <sstream>
#include <cassert>

#include "chemfiles/formats/XYZ.hpp"

#include "chemfiles/Error.hpp"
#include "chemfiles/Logger.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/File.hpp"

using namespace chemfiles;

XYZFormat::XYZFormat(File& f):
    Format(f), textfile_(dynamic_cast<TextFile&>(file_)), step_cursor_(0) {}

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
            line = textfile_.getline();
            auto natoms = std::stoul(line);
            textfile_.readlines(natoms + 1);
            step_cursor_++;
        } catch (const std::invalid_argument&) {
            // We could not read an integer, so give up here
            return false;
        } catch (const FileError&) {
            // We could not read the lines from the file
            throw FormatError(
                "Not enough lines in '" + textfile_.filename() +
                "' for XYZ format at step " + std::to_string(i)
            );
        }
        i++;
    }
    return true;
}

size_t XYZFormat::nsteps() {
    textfile_.rewind();
    size_t n = 0;
    while (!textfile_.eof()) {
        if (forward(1))
            n++;
    }
    textfile_.rewind();
    return n;
}

void XYZFormat::read_step(const size_t step, Frame& frame){
    textfile_.rewind();
    forward(step);
    read(frame);
}

void XYZFormat::read(Frame& frame){
    size_t natoms;

    try {
        natoms = std::stoul(textfile_.getline());
        textfile_.getline(); // XYZ comment line;
    } catch (const std::exception& e) {
        throw FormatError("Can not read next step: " + string(e.what()));
    }

    std::vector<std::string> lines(natoms);

    try {
        lines = textfile_.readlines(natoms);
    }
    catch (const FileError& e) {
        throw FormatError("Can not read file: " + string(e.what()));
    }

    frame.resize(natoms);
    auto positions = frame.positions();
    auto& topology = frame.topology();
    for (size_t i=0; i<lines.size(); i++) {
        std::istringstream string_stream;
        float x, y, z;
        string name;

        string_stream.str(lines[i]);
        string_stream >> name >> x >> y >> z ;
        positions[i][0] = x;
        positions[i][1] = y;
        positions[i][2] = z;
        topology[i] = Atom(name);
    }
}

void XYZFormat::write(const Frame& frame){
    auto& topology = frame.topology();
    auto& positions = frame.positions();
    assert(frame.natoms() == topology.natoms());

    textfile_ << frame.natoms() << "\n";
    textfile_ << "Written by the chemfiles library\n";

    for (size_t i=0; i<frame.natoms(); i++){
        auto pos = positions[i];
        auto name = topology[i].name();
        if (name == "")
            name = "X";
        textfile_ << name   << " "
                  << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
    }
}
