// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto B = Matrix3D(11, 22, 33);
    assert(B[0][0] == 11);
    assert(B[0][1] == 0);
    assert(B[0][2] == 0);

    assert(B[1][0] == 0);
    assert(B[1][1] == 22);
    assert(B[1][2] == 0);

    assert(B[2][0] == 0);
    assert(B[2][1] == 0);
    assert(B[2][2] == 33);
    // [example]
}
