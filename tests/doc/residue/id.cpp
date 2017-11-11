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
    assert(!residue.id());

    residue = Residue("BAR", 33);
    assert(residue.id());
    assert(residue.id().value() == 33);
    // [example]
}
