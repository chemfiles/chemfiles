// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
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
    assert(Vector3D(1, 0, 1) + Vector3D(0, 2, 1) == Vector3D(1, 2, 2));
    assert(Vector3D(1, 0, 1) - Vector3D(0, 2, 1) == Vector3D(1, -2, 0));

    // product and division by a scalar
    assert(Vector3D(1, 0, 1) * 5 == Vector3D(5, 0, 5));
    assert(5 * Vector3D(1, 0, 1) == Vector3D(5, 0, 5));
    assert(Vector3D(2, 0, 3) / 3 == Vector3D(2.0/3, 0, 1));
    // [example]
    return 0;
}
