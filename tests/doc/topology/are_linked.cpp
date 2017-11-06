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

    auto second = Residue("second");
    second.add_atom(1);
    topology.add_residue(second);

    assert(!topology.are_linked(first, second));

    topology.add_bond(0, 1);
    assert(topology.are_linked(first, second));
    // [example]
}
