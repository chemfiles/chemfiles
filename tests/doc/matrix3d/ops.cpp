// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    // equality and inequality
    assert(Matrix3D() == Matrix3D(0, 0, 0));
    assert(Matrix3D(1, 2, 3) != Matrix3D(0, 0, 0));

    // Indexing
    auto M = Matrix3D(
        11, 12, 13,
        21, 22, 23,
        31, 32, 33
    );
    assert(M[0][0] == 11);
    assert(M[0][1] == 12);
    assert(M[0][2] == 13);

    assert(M[1][0] == 21);
    assert(M[1][1] == 22);
    assert(M[1][2] == 23);

    assert(M[2][0] == 31);
    assert(M[2][1] == 32);
    assert(M[2][2] == 33);

    // Matrix-vector multiplication
    auto A = Matrix3D(1, 3, 2);
    auto v = Vector3D(1, 1, 1);
    assert(A * v == Vector3D(1, 3, 2));

    // Matrix-Matrix multiplication
    auto B = Matrix3D(0.5, 2, 1);
    assert(A * B == Matrix3D(0.5, 6, 2));
    // [example]
    return 0;
}
