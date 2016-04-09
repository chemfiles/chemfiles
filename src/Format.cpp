/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemfiles/Format.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

void Format::read_step(size_t, Frame&){
    throw FormatError("Operation 'read_step' is not implemented for " + description());
}

void Format::read(Frame&){
    throw FormatError("Operation 'read' is not implemented for " + description());
}

void Format::write(const Frame&){
    throw FormatError("Operation 'write' is not implemented for " + description());
}
