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
    frame.add_atom(Atom("H"), {3.0, 4.0, 5.0});
    frame.add_atom(Atom("O"), {1.0, -2.0, 3.0});
    frame.add_atom(Atom("H"), {1.3, 0.0, -1.0});

    auto positions = frame.positions();
    assert(positions.size() == 3);

    // Indexing the positions
    assert(positions[0] == Vector3D(3.0, 4.0, 5.0));
    assert(positions[1] == Vector3D(1.0, -2.0, 3.0));
    assert(positions[2] == Vector3D(1.3, 0.0, -1.0));

    // Iteration on positions
    for (auto& position: positions) {
        position[0] += 1.0;
        position[2] -= 1.0;
    }

    assert(positions[0] == Vector3D(4.0, 4.0, 4.0));
    assert(positions[1] == Vector3D(2.0, -2.0, 2.0));
    assert(positions[2] == Vector3D(2.3, 0.0, -2.0));
    // [example]
}
