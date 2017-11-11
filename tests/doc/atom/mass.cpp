// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto atom = Atom("C");
    assert(atom.mass() == 12.011);

    atom.set_mass(42.5);
    assert(atom.mass() == 42.5);
    // [example]
}
