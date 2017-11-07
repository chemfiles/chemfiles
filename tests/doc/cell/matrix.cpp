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
    auto matrix = cell.matrix();

    assert(matrix[0][0] == 11);
    assert(matrix[1][1] == 22);
    assert(matrix[2][2] == 33);

    assert(fabs(matrix[0][1]) < 1e-12);
    assert(fabs(matrix[0][2]) < 1e-12);
    assert(fabs(matrix[1][2]) < 1e-12);

    assert(matrix[1][0] == 0);
    assert(matrix[2][0] == 0);
    assert(matrix[2][1] == 0);
    // [example]
}
