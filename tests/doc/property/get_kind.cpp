// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    assert(Property(42).get_kind() == Property::DOUBLE);
    assert(Property(false).get_kind() == Property::BOOL);
    assert(Property("string").get_kind() == Property::STRING);
    assert(Property(Vector3D(1, 2, 3)).get_kind() == Property::VECTOR3D);
    // [example]
}
