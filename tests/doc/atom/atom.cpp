// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto atom = Atom();
    assert(atom.name() == "");
    assert(atom.type() == "");

    atom = Atom("Zn");
    assert(atom.name() == "Zn");
    assert(atom.type() == "Zn");
    // The atom mass is automatically set
    assert(atom.mass() == 65.38);

    atom = Atom("O23", "Ow");
    assert(atom.name() == "O23");
    assert(atom.type() == "Ow");
    // The atom mass is not automatically set, "Ow" is not in the periodic table
    assert(atom.mass() == 0.0);

    // [example]
}
