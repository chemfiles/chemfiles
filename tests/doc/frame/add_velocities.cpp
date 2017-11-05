// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    // Default constructed frames does not contains velocities
    assert(!frame.velocities());
    frame.add_velocities();

    assert(frame.velocities());
    // [example]
    return 0;
}
