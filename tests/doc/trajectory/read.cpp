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
    auto trajectory = Trajectory("water.nc");

    auto frame = trajectory.read();
    // Use the frame for awesome science here!

    // This is one way to iterate over all the frames in a trajectory
    while (!trajectory.done()) {
        frame = trajectory.read();
        // ...
    }
    // [example]
}
