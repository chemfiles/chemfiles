/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include <sstream>
#include <cassert>

#include "formats/XYZ.hpp"

#include "Error.hpp"
#include "Logger.hpp"
#include "Frame.hpp"
#include "files/File.hpp"

using namespace harp;

std::string XYZFormat::description() const {
    return "XYZ file format.";
}

// Quick forward the file for nsteps, return false if the end of file (eof) was
// reach before the end.
static bool forward(TextFile* file, size_t nsteps) {
    size_t i=0;
    // Move the file pointer to the good position step by step, as the number of
    // atoms may not be constant
    std::string line;
    while (i < nsteps){
        try {
            line = file->getline();
            auto natoms = std::stoul(line);
            file->readlines(natoms + 1);
        } catch (const std::exception& e) {
            // handling single new line at the end of the file
            if (line == "" && file->eof())
                return false;
            else
                throw FormatError("Can not read step: " + string(e.what()));
        }
        i++;
    }
    return true;
}

size_t XYZFormat::nsteps(File* file) const {
    auto textfile = dynamic_cast<TextFile*>(file);
    textfile->rewind();
    size_t n = 0;
    while (not textfile->eof()) {
        if (forward(textfile, 1))
            n++;
    }
    textfile->rewind();
    return n;
}

void XYZFormat::read_at_step(File* file, const size_t step, Frame& frame){
    auto textfile = dynamic_cast<TextFile*>(file);
    textfile->rewind();
    forward(textfile, step);
    read_next_step(file, frame);
}

void XYZFormat::read_next_step(File* file, Frame& frame){
    auto textfile = dynamic_cast<TextFile*>(file);
    size_t natoms;

    try {
        natoms = std::stoul(textfile->getline());
        textfile->getline(); // XYZ comment line;
    } catch (const std::exception& e) {
        throw FormatError("Can not read next step: " + string(e.what()));
    }

    std::vector<std::string> lines(natoms);

    try {
        lines = textfile->readlines(natoms);
    }
    catch (const FileError& e) {
        throw FormatError("Can not read file: " + string(e.what()));
    }

    frame.topology().clear();
    frame.resize(natoms);

    for (size_t i=0; i<lines.size(); i++) {
        std::istringstream string_stream;
        float x, y, z;
        string name;

        string_stream.str(lines[i]);
        string_stream >> name >> x >> y >> z ;
        frame.positions()[i] = Vector3D(x, y, z);
        frame.topology().append(Atom(name));
    }
    frame.topology().guess_bonds();
}

void XYZFormat::write_step(File* file, const Frame& frame){
    auto textfile = dynamic_cast<TextFile*>(file);

    const auto topology = frame.topology();
    const auto positions = frame.positions();
    assert(frame.natoms() == topology.natoms());

    *textfile << frame.natoms() << "\n";
    *textfile << "Written by Chemharp\n";

    for (size_t i=0; i<frame.natoms(); i++){
        auto& pos = positions[i];
        auto name = topology[i].name();
        if (name == "")
            name = "X";
        *textfile << name   << " "
                  << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
    }
}


// Register the xyz format with the ".xyz" extension and the "XYZ" description.
REGISTER(XYZFormat, "XYZ");
REGISTER_EXTENSION(XYZFormat, ".xyz");
