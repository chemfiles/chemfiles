// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto angle = Angle(55, 23, 12);

    // indexing
    assert(angle[0] == 12);
    assert(angle[1] == 23);
    assert(angle[2] == 55);

    // equality
    assert(angle == Angle(12, 23, 55));
    assert(angle != Angle(12, 23, 24));

    // lexicographic comparison
    assert(angle < Angle(44, 23, 55));
    assert(angle >= Angle(11, 33, 14));
    // [example]
}
