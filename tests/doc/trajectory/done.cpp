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

    while (!trajectory.done()) {
        auto frame = trajectory.read();
    }

    // When using `read_step`, `done()` returns `true` if the most recent call
    // was used to read the last step (`nsteps() - 1`).
    auto frame = trajectory.read_step(0);
    assert(!trajectory.done());

    auto nsteps = trajectory.nsteps();
    frame = trajectory.read_step(nsteps - 1);
    assert(trajectory.done());
    // [example]
}
