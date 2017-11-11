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
    auto trajectory = Trajectory("water.xyz");
    trajectory.set_cell(UnitCell(11, 11, 22));

    auto frame = trajectory.read();
    // The frame cell is now an orthorhombic cell with lengths of
    // 11 A, 11 A and 22 A, regardless of what was defined in the file.

    // [example]
}
