// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto topology = Topology();
    topology.add_atom(Atom("Zn"));
    topology.add_atom(Atom("Fe"));

    assert(topology.residues().size() == 0);

    auto residue = Residue("first");
    residue.add_atom(0);
    topology.add_residue(residue);

    assert(topology.residues().size() == 1);
    // [example]
}
