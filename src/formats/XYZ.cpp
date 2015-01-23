/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "formats/XYZ.hpp"
#include "Frame.hpp"

using namespace harp;

XYZReader::XYZReader(){
    // TODO
}

XYZReader::~XYZReader(){
    // TODO
}

Frame& XYZReader::read_at_step(const File& file, const int step){
    // TODO
    Frame frame=Frame();
    return frame;
}

Frame& XYZReader::read_next_step(const File& file, const int step){
    // TODO
    Frame frame=Frame();
    return frame;
}

REGISTER_READER(XYZReader, ".xyz")
