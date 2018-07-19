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
        3, 0, 0,
        0, 2, 0,
        0, 0, 1
    );
    assert(A.determinant() == 6);
    // [example]
}
