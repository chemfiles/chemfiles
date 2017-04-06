// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

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
