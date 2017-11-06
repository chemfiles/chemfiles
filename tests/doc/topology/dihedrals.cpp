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
    topology.add_atom(Atom("O"));
    topology.add_atom(Atom("H"));

    topology.add_bond(0, 1);
    topology.add_bond(1, 2);
    topology.add_bond(2, 3);

    assert(topology.dihedrals() == std::vector<Dihedral>({{0, 1, 2, 3}}));

    auto dihedrals = topology.dihedrals();
    // perform a binary search in the dihedrals
    auto it = std::lower_bound(dihedrals.begin(), dihedrals.end(), Dihedral(0, 1, 2, 3));
    assert(it != dihedrals.end());
    assert(*it == Dihedral(0, 1, 2, 3));
    // [example]
}
