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
    topology.add_atom(Atom("H"));
    topology.add_atom(Atom("O"));
    topology.add_atom(Atom("H"));

    topology.add_bond(0, 1);
    topology.add_bond(1, 2);

    assert(topology.bonds().size() == 2);
    assert(topology.angles().size() == 1);

    topology.clear_bonds();
    assert(topology.bonds().size() == 0);
    assert(topology.angles().size() == 0);
    // [example]
}
