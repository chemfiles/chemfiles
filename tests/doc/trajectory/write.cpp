// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [no-run]
    // [example]
    auto trajectory = Trajectory("water.nc", 'w');

    auto frame = Frame();
    frame.add_atom(Atom("O"), {0, 0, 0});
    frame.add_atom(Atom("H"), {1, 0, 0});
    frame.add_atom(Atom("H"), {0, 1, 0});

    trajectory.write(frame);
    // [example]
}
