// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    // Building a frame containing a Cl2 molecule
    auto frame = Frame();
    frame.add_atom(Atom("Cl"), {0.0, 0.0, 0.0});
    frame.add_atom(Atom("Cl"), {2.0, 0.0, 0.0});

    assert(frame.topology().bonds().size() == 0);

    frame.guess_topology();
    assert(frame.topology().bonds().size() == 1);
    // [example]
}
