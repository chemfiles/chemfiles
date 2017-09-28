// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto u = Vector3D(0, 1, 2);
    auto v = Vector3D(1, 0, 2);

    assert(cross(u, v) == Vector3D(2, 2, -1));
    // [example]
    return 0;
}
