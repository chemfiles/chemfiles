// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto v = Vector3D(1.0, 1.0, 1.0);
    assert(v.norm() == sqrt(3));
    // [example]
    return 0;
}
