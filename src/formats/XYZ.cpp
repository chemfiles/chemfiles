/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include <sstream>

#include "formats/XYZ.hpp"

#include "Error.hpp"
#include "Frame.hpp"
#include "files/File.hpp"

using namespace harp;

std::string XYZFormat::description() const {
    return "XYZ file format.";
}

void XYZFormat::read_at_step(shared_ptr<File> file, const size_t step, Frame& frame){
    auto textfile = std::dynamic_pointer_cast<TextFile>(file);
    textfile->rewind();
    size_t i=0;
    while (i < step - 1){
        // Move the file pointer to the good position
        try {
            size_t natoms = std::stoul(textfile->getline());
            textfile->readlines(natoms+1);
        }
        catch (const HarpFileError& e) {
            throw HarpFormatError("Can not read step " + std::to_string(step) +
                                  ": " + e.what());
        }
        i += 1;
    }
    read_next_step(file, frame);
}

void XYZFormat::read_next_step(shared_ptr<File> file, Frame& frame){
    auto textfile = std::dynamic_pointer_cast<TextFile>(file);
    size_t natoms = std::stoul(textfile->getline());

    textfile->getline(); // XYZ comment line;
    std::vector<std::string> lines;
    lines.reserve(natoms);

    try {
        lines = textfile->readlines(natoms);
    }
    catch (const HarpFileError& e) {
        throw HarpFormatError("Can not read file: " + string(e.what()));
    }

    frame.topology().clear();
    frame.reserve(natoms);

    for (size_t i=0; i<lines.size(); i++) {
        std::istringstream string_stream;
        float x, y, z;
        string name;

        string_stream.str(lines[i]);
        string_stream >> name >> x >> y >> z ;
        frame.positions()[i] = Vector3D(x, y, z);
        frame.topology().add_atom(Atom(name));
    }
    frame.topology().guess_bonds();
}

// Register the xyz format with the ".xyz" extension and the "XYZ" description.
REGISTER(XYZFormat, "XYZ");
REGISTER_EXTENSION(XYZFormat, ".xyz");
