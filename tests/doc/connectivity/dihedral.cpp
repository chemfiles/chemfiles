// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto dihedral = Dihedral(2, 55, 23, 12);

    // indexing
    assert(dihedral[0] == 12);
    assert(dihedral[1] == 23);
    assert(dihedral[2] == 55);
    assert(dihedral[3] == 2);

    // equality
    assert(dihedral == Dihedral(12, 23, 55, 2));
    assert(dihedral != Dihedral(12, 23, 24, 2));

    // lexicographic comparison
    assert(dihedral < Dihedral(44, 23, 55, 1));
    assert(dihedral >= Dihedral(11, 33, 14, 4));
    // [example]
}
