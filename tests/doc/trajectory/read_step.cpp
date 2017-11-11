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

    auto frame = trajectory.read_step(4);
    // Use the frame for awesome science here!

    // This is one way to iterate over all the frames in a trajectory
    for (size_t i = 0; i < trajectory.nsteps(); i++) {
        frame = trajectory.read_step(i);
        // ...
    }
    // [example]
}
