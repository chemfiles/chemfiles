// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <typeinfo>

#include "chemfiles/Format.hpp"
#include "chemfiles/ErrorFmt.hpp"
using namespace chemfiles;

void Format::read_step(size_t /*unused*/, Frame& /*unused*/) {
    throw format_error(
        "'read_step' is not implemented for this format ({})",
        typeid(*this).name()
    );
}

void Format::read(Frame& /*unused*/) {
    throw format_error(
        "'read' is not implemented for this format ({})",
        typeid(*this).name()
    );
}

void Format::write(const Frame& /*unused*/) {
    throw format_error(
        "'write' is not implemented for this format ({})",
        typeid(*this).name()
    );
}
