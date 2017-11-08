// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto property = Property(false);

    assert(property.get_kind() == Property::BOOL);
    assert(property.as_bool() == false);
    // [example]
}
