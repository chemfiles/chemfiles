// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto cell = UnitCell({1, 1, 1}, {60, 80, 123});

    // due to the way unit cell is stored, there can be a few floating point 
    // rounding error when accessing angles
    auto angles = cell.angles();
    assert(fabs(angles[0] - 60) < 1e-12);
    assert(fabs(angles[1] - 80) < 1e-12);
    assert(fabs(angles[2] - 123) < 1e-12);

    cell.set_angles({91, 92, 93});
    angles = cell.angles();
    assert(fabs(angles[0] - 91) < 1e-12);
    assert(fabs(angles[1] - 92) < 1e-12);
    assert(fabs(angles[2] - 93) < 1e-12);
    // [example]
}
