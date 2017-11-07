// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto cell = UnitCell(32);

    assert(cell.shape() == UnitCell::ORTHORHOMBIC);

    assert(cell.a() == 32);
    assert(cell.b() == 32);
    assert(cell.c() == 32);

    assert(cell.alpha() == 90);
    assert(cell.beta() == 90);
    assert(cell.gamma() == 90);

    // [example]
}
