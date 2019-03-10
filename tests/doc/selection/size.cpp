// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    assert(Selection("type H and index > 254").size() == 1);
    assert(Selection("two: name(#1) H and type(#2) Ow").size() == 2);
    assert(Selection("dihedrals: name(#1) H and name(#3) O").size() == 4);
    // [example]
}
