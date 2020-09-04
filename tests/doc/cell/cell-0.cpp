// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto cell = UnitCell();

    assert(cell.shape() == UnitCell::INFINITE);

    assert(cell.lengths() == Vector3D(0, 0, 0));
    assert(cell.angles() == Vector3D(90, 90, 90));
    // [example]
}
