// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto property = Property(Vector3D(11, 22, 33));
    assert(property.as_vector3d() == Vector3D(11, 22, 33));
    // [example]
}
