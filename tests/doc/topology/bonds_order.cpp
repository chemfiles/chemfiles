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
    topology.add_atom(Atom("O"));
    topology.add_atom(Atom("O"));

    topology.add_bond(0, 1, Bond::SINGLE);
    topology.add_bond(3, 4, Bond::DOUBLE);
    topology.add_bond(1, 2, Bond::SINGLE);

    // Lookup by the bond index
    assert(topology.bond_orders()[0] == Bond::SINGLE);
    assert(topology.bond_orders()[1] == Bond::SINGLE);
    assert(topology.bond_orders()[2] == Bond::DOUBLE);

    // Lookup by the atom indexes
    assert(topology.bond_order(0, 1) == Bond::SINGLE);
    assert(topology.bond_order(3, 4) == Bond::DOUBLE);

    // [example]
}
