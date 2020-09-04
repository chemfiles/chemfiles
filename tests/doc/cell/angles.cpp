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

    assert(cell.angles() == Vector3D(60, 80, 123));

    cell.set_angles({91, 92, 93});
    assert(cell.angles() == Vector3D(91, 92, 93));
    // [example]
}
