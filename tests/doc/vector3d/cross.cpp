// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto u = Vector3D(0, 1, 2);
    auto v = Vector3D(1, 0, 2);

    assert(cross(u, v) == Vector3D(2, 2, -1));
    // [example]
}
