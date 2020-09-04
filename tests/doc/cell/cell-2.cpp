// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto cell = UnitCell({11, 22, 33}, {80, 90, 70});

    assert(cell.shape() == UnitCell::TRICLINIC);

    assert(cell.lengths() == Vector3D(11, 22, 33));
    assert(cell.angles() == Vector3D(80, 90, 70));
    // [example]
}
