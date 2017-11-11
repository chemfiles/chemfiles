// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto improper = Improper(2, 55, 23, 12);

    // indexing
    assert(improper[0] == 2);
    assert(improper[1] == 55);
    assert(improper[2] == 12);
    assert(improper[3] == 23);

    // equality
    assert(improper == Improper(12, 55, 2, 23));
    assert(improper != Improper(12, 55, 2, 21));

    // lexicographic comparison
    assert(improper < Improper(44, 23, 55, 8));
    assert(improper >= Improper(11, 33, 14, 1));
    // [example]
}
