// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    assert(Matrix3D(3, 2, 1).invert() == Matrix3D(1.0/3.0, 1.0/2.0, 1.0));
    // [example]
    return 0;
}
