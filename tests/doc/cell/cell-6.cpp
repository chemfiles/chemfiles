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

    assert(cell.shape() == UnitCell::ORTHORHOMBIC);

    assert(cell.a() == 11);
    assert(cell.b() == 22);
    assert(cell.c() == 33);

    assert(cell.alpha() == 90);
    assert(cell.beta() == 90);
    assert(cell.gamma() == 90);

    // [example]
}
