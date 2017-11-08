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
    assert(Selection("pairs: name(#1) H and type(#2) Ow").size() == 2);
    assert(Selection("four: (name(#1) H and type(#2) Ow) or (name(#3) Ow and type(#4) H)").size() == 4);
    // [example]
}
