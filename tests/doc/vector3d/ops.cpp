// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    // equality and inequality
    assert(Vector3D() == Vector3D(0, 0, 0));
    assert(Vector3D(1, 2, 3) != Vector3D(0, 0, 0));

    // Indexing
    auto u = Vector3D(1.5, 2.0, -3.0);
    assert(u[0] == 1.5);
    assert(u[1] == 2.0);
    assert(u[2] == -3.0);

    // sum and difference
    auto sum = Vector3D(1, 0, 1) + Vector3D(0, 2, 1);
    assert(sum == Vector3D(1, 2, 2));

    auto diff = Vector3D(1, 0, 1) - Vector3D(0, 2, 1);
    assert(diff == Vector3D(1, -2, 0));

    // product and division by a scalar
    auto rhs = Vector3D(1, 0, 1) * 5;
    assert(rhs == Vector3D(5, 0, 5));

    auto lhs = 5 * Vector3D(1, 0, 1);
    assert(lhs == Vector3D(5, 0, 5));

    auto div = Vector3D(2, 0, 3) / 3;
    assert(div == Vector3D(2.0/3, 0, 1));
    // [example]
}
