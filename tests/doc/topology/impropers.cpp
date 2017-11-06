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
    topology.add_atom(Atom("C"));
    topology.add_atom(Atom("H"));
    topology.add_atom(Atom("H"));
    topology.add_atom(Atom("H"));

    topology.add_bond(0, 1);
    topology.add_bond(0, 2);
    topology.add_bond(0, 3);

    assert(topology.impropers() == std::vector<Improper>({{1, 0, 2, 3}}));

    auto impropers = topology.impropers();
    // perform a binary search in the impropers
    auto it = std::lower_bound(impropers.begin(), impropers.end(), Improper(1, 0, 2, 3));
    assert(it != impropers.end());
    assert(*it == Improper(1, 0, 2, 3));
    // [example]
}
