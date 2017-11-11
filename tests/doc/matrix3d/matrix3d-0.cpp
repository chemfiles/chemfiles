// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto C = Matrix3D();
    assert(C[0][0] == 0);
    assert(C[0][1] == 0);
    assert(C[0][2] == 0);

    assert(C[1][0] == 0);
    assert(C[1][1] == 0);
    assert(C[1][2] == 0);

    assert(C[2][0] == 0);
    assert(C[2][1] == 0);
    assert(C[2][2] == 0);
    // [example]
}
