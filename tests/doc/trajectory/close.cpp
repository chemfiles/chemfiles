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

    auto frame = Frame();
    // setup the frame

    trajectory.write(frame);
    trajectory.close();
    // [example]
}
