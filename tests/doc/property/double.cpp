// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto property = Property(45.2);

    assert(property.get_kind() == Property::DOUBLE);
    assert(property.as_double() == 45.2);

    // Various overload convert integers to double
    assert(Property(45).get_kind() == Property::DOUBLE);     // int
    assert(Property(45l).get_kind() == Property::DOUBLE);    // long
    assert(Property(45ll).get_kind() == Property::DOUBLE);   // long long
    assert(Property(45u).get_kind() == Property::DOUBLE);    // unsigned
    assert(Property(45ul).get_kind() == Property::DOUBLE);   // unsigned long
    assert(Property(45ull).get_kind() == Property::DOUBLE);  // unsigned long long
    // [example]
}
