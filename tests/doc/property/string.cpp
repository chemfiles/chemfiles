// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    // from a std::string
    auto property = Property(std::string("foo"));

    assert(property.get_kind() == Property::STRING);
    assert(property.as_string() == "foo");

    // from a const char*
    property = Property("bar");

    assert(property.get_kind() == Property::STRING);
    assert(property.as_string() == "bar");
    // [example]
}
