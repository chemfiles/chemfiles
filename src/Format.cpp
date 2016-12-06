// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/Format.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

void Format::read_step(size_t /*unused*/, Frame& /*unused*/) {
    throw FormatError("Operation 'read_step' is not implemented for " +
                      description());
}

void Format::read(Frame& /*unused*/) {
    throw FormatError("Operation 'read' is not implemented for " +
                      description());
}

void Format::write(const Frame& /*unused*/) {
    throw FormatError("Operation 'write' is not implemented for " +
                      description());
}
