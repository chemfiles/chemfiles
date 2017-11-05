// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto A = Matrix3D(
        11, 12, 13,
        21, 22, 23,
        31, 32, 33
    );
    assert(A[0][0] == 11);
    assert(A[0][1] == 12);
    assert(A[0][2] == 13);

    assert(A[1][0] == 21);
    assert(A[1][1] == 22);
    assert(A[1][2] == 23);

    assert(A[2][0] == 31);
    assert(A[2][1] == 32);
    assert(A[2][2] == 33);
    // [example]
}
