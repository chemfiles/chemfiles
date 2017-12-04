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

    assert(cell.a() == 11);
    assert(cell.b() == 22);
    assert(cell.c() == 33);

    cell.set_a(111);
    cell.set_b(222);
    cell.set_c(333);

    assert(cell.a() == 111);
    assert(cell.b() == 222);
    assert(cell.c() == 333);
    // [example]
}
