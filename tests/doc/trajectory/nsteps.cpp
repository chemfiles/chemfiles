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

    auto nsteps = trajectory.nsteps();
    for (size_t i = 0; i < nsteps; i++) {
        auto frame = trajectory.read_step(i);
    }

    // [example]
}
