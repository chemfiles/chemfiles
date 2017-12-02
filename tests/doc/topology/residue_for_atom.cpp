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

    auto first = Residue("first");
    first.add_atom(0);
    topology.add_residue(first);

    assert(topology.residue_for_atom(0));
    assert(topology.residue_for_atom(0)->name() == "first");

    assert(!topology.residue_for_atom(1));
    assert(topology.residue_for_atom(1) == nullopt);
    // [example]
}
