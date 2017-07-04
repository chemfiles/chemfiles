// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <typeinfo>

#include "chemfiles/Format.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

void Format::read_step(size_t /*unused*/, Frame& /*unused*/) {
    throw FormatError(
        "'read_step' is not implemented for this format (" +
        std::string(typeid(*this).name()) + ")"
    );
}

void Format::read(Frame& /*unused*/) {
    throw FormatError(
        "'read' is not implemented for this format (" +
        std::string(typeid(*this).name()) + ")"
    );
}

void Format::write(const Frame& /*unused*/) {
    throw FormatError(
        "'write' is not implemented for this format (" +
        std::string(typeid(*this).name()) + ")"
    );
}
