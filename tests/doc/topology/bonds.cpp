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

    assert(topology.bonds() == std::vector<Bond>({{0, 1}, {1, 2}}));

    auto bonds = topology.bonds();
    // perform a binary search in the bonds
    auto it = std::lower_bound(bonds.begin(), bonds.end(), Bond(1, 2));
    assert(it != bonds.end());
    assert(*it == Bond(1, 2));
    // [example]
}
