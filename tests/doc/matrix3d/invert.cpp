// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    assert(Matrix3D(3, 2, 1).invert() == Matrix3D(1.0/3.0, 1.0/2.0, 1.0));
    // [example]
}
