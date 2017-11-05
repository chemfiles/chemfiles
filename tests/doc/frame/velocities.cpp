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

    // adding a few atoms with velocity data
    frame.add_atom(Atom("H"), {}, {3.0, 4.0, 5.0});
    frame.add_atom(Atom("O"), {}, {1.0, -2.0, 3.0});
    frame.add_atom(Atom("H"), {}, {1.3, 0.0, -1.0});

    auto velocities = *frame.velocities();
    assert(velocities.size() == 3);

    // Indexing the velocities
    assert(velocities[0] == Vector3D(3.0, 4.0, 5.0));
    assert(velocities[1] == Vector3D(1.0, -2.0, 3.0));
    assert(velocities[2] == Vector3D(1.3, 0.0, -1.0));

    // Iteration on positions
    for (auto& velocity: velocities) {
        velocity[0] += 1.0;
        velocity[2] -= 1.0;
    }

    assert(velocities[0] == Vector3D(4.0, 4.0, 4.0));
    assert(velocities[1] == Vector3D(2.0, -2.0, 2.0));
    assert(velocities[2] == Vector3D(2.3, 0.0, -2.0));
    // [example]
}
