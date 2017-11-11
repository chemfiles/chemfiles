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

    assert(topology.angles() == std::vector<Angle>({{0, 1, 2}, {1, 2, 3}}));

    auto angles = topology.angles();
    // perform a binary search in the angles
    auto it = std::lower_bound(angles.begin(), angles.end(), Angle(1, 2, 3));
    assert(it != angles.end());
    assert(*it == Angle(1, 2, 3));
    // [example]
}
