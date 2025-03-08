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

    // When using `read_at`, `done()` returns `true` if the most recent call
    // was used to read the last step (`size() - 1`).
    auto frame = trajectory.read_at(0);
    assert(!trajectory.done());

    auto size = trajectory.size();
    frame = trajectory.read_at(size - 1);
    assert(trajectory.done());
    // [example]
}
