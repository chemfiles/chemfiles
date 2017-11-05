// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto frame = Frame();
    // Default constructed frames does not contains velocities
    assert(!frame.velocities());
    frame.add_velocities();

    assert(frame.velocities());
    // [example]
}
