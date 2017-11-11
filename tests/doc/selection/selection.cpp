// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto selection = Selection("name H or mass < 67");

    assert(selection.size() == 1);
    assert(selection.string() == "name H or mass < 67");
    // [example]
}
