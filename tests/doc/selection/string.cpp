// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto selection = Selection("angles: name(#2) O and mass(#1) < 3.4");
    assert(selection.string() == "angles: name(#2) O and mass(#1) < 3.4");
    // [example]
}
