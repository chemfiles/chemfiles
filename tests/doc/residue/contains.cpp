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

    residue.add_atom(2);
    residue.add_atom(22);
    residue.add_atom(42);
    residue.add_atom(36);

    assert(residue.contains(22) == true);
    assert(residue.contains(23) == false);
    // [example]
}
