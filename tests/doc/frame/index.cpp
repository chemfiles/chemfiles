// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto frame = Frame();
    frame.add_atom(Atom("Co"), {0.0, 0.0, 0.0});
    frame.add_atom(Atom("V"), {1.0, 0.0, 0.0});
    frame.add_atom(Atom("Fe"), {0.0, 2.0, 0.0});
    frame.add_atom(Atom("Fe"), {0.0, 0.0, 3.0});

    assert(frame[0].name() == "Co");
    assert(frame[1].name() == "V");

    frame[2].set_mass(45);
    assert(frame[2].mass() == 45);
    // [example]
}
