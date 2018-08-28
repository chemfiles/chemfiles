// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    assert(Property(42).kind() == Property::DOUBLE);
    assert(Property(false).kind() == Property::BOOL);
    assert(Property("string").kind() == Property::STRING);
    assert(Property(Vector3D(1, 2, 3)).kind() == Property::VECTOR3D);
    // [example]
}
