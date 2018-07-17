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
        3, 0, 5,
        1, 2, 6,
        2, 0, 1
    );
    auto transposed = Matrix3D(
        3, 1, 2,
        0, 2, 0,
        5, 6, 1
    );
    assert(A.transpose() == transposed);
    // [example]
}
