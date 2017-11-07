// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto cell = UnitCell(11, 22, 33);
    assert(cell.wrap(Vector3D(14, -12, 5)) == Vector3D(3, 10, 5));
    // [example]
}
