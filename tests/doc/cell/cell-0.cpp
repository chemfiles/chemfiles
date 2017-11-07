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

    assert(cell.a() == 0);
    assert(cell.b() == 0);
    assert(cell.c() == 0);

    assert(cell.alpha() == 90);
    assert(cell.beta() == 90);
    assert(cell.gamma() == 90);

    // [example]
}
