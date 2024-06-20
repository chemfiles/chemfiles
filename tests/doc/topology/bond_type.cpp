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

    topology.add_bond(0, 1, Bond::SINGLE, "single");
    topology.add_bond(3, 4, Bond::DOUBLE, "double");
    topology.add_bond(1, 2, Bond::SINGLE, "single");

    // Lookup by the bond index
    assert(topology.bond_types()[0] == "single");
    assert(topology.bond_types()[1] == "single");
    assert(topology.bond_types()[2] == "double");

    // Lookup by the atom indexes
    assert(topology.bond_type(0, 1) == "single");
    assert(topology.bond_type(3, 4) == "double");

    // [example]
}
