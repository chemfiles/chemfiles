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
    assert(atom.charge() == 0.0);

    atom.set_charge(0.2);
    assert(atom.charge() == 0.2);
    // [example]
}
