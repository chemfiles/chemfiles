// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto atom = Atom("C1");
    assert(atom.type() == "C1");

    atom.set_type("C");
    assert(atom.type() == "C");

    atom = Atom("O1", "O");
    assert(atom.type() == "O");
    // [example]
}
