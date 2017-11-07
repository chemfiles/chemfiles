// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto cell = UnitCell(1, 1, 1, 60, 80, 123);

    assert(cell.alpha() == 60);
    assert(cell.beta() == 80);
    assert(cell.gamma() == 123);

    cell.set_alpha(91);
    cell.set_beta(92);
    cell.set_gamma(93);

    assert(cell.alpha() == 91);
    assert(cell.beta() == 92);
    assert(cell.gamma() == 93);
    // [example]
}
