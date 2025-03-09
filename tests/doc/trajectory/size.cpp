// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

TEST_CASE() {
    // [no-run]
    // [example]
    auto trajectory = Trajectory("water.nc");

    auto size = trajectory.size();
    for (size_t i = 0; i < size; i++) {
        auto frame = trajectory.read_at(i);
    }

    // [example]
}
