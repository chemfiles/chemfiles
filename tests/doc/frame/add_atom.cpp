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
    // add atom without velocities
    frame.add_atom(Atom("H"), Vector3D(3.0, 4.0, 5.0));

    frame.add_velocities();
    // add atom with velocities
    frame.add_atom(Atom("O"), Vector3D(0.0, 0.0, 0.0), Vector3D(1.0, 2.0, 0.0));

    auto velocities = *frame.velocities();
    assert(velocities[0] == Vector3D(0.0, 0.0, 0.0));
    assert(velocities[1] == Vector3D(1.0, 2.0, 0.0));

    // using brace initialization for smaller code
    frame.add_atom(Atom("H"), {1.0, 2.0, 0.0}, {0.0, -0.4, 0.3});
    // [example]
}
