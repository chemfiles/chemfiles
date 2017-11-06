// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto bond = Bond(55, 23);

    // indexing
    assert(bond[0] == 23);
    assert(bond[1] == 55);

    // equality
    assert(bond == Bond(23, 55));
    assert(bond != Bond(23, 24));

    // lexicographic comparison
    assert(bond < Bond(44, 55));
    assert(bond >= Bond(12, 33));
    // [example]
}
