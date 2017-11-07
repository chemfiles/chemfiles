// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto residue = Residue("FOO");
    assert(residue.size() == 0);

    residue.add_atom(2);
    residue.add_atom(22);
    residue.add_atom(42);
    residue.add_atom(36);

    assert(residue.size() == 4);
    // [example]
}
