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
    frame.add_atom(Atom("Zn"), {0.0, 0.0, 0.0});
    frame.add_atom(Atom("Fe"), {1.0, 2.0, 3.0});

    auto residue = Residue("first");
    residue.add_atom(0);
    frame.add_residue(residue);

    // residues are actually stored in the topology
    assert(frame.topology().residues().size() == 1);
    // [example]
}
