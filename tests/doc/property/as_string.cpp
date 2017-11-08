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
    auto property = Property("foo");
    assert(property.as_string() == "foo");
    // [example]
}
