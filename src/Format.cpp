/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Format.hpp"
#include "Frame.hpp"
#include "Error.hpp"
using namespace harp;

void Format::read_at(File*, const size_t, Frame&){
    throw FormatError("Not implemented function read.");
}

void Format::read(File*, Frame&){
    throw FormatError("Not implemented function read.");
}

void Format::write(File*, const Frame&){
    throw FormatError("Not implemented function read.");
}
