// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto cell = UnitCell({11, 22, 33});
    assert(cell.shape() == UnitCell::ORTHORHOMBIC);

    cell.set_shape(UnitCell::TRICLINIC);
    assert(cell.shape() == UnitCell::TRICLINIC);
    // [example]
}
