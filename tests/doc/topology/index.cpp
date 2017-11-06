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
    topology.add_atom(Atom("Co"));
    topology.add_atom(Atom("V"));
    topology.add_atom(Atom("Fe"));
    topology.add_atom(Atom("Fe"));

    assert(topology[0].name() == "Co");
    assert(topology[1].name() == "V");

    topology[2].set_mass(45);
    assert(topology[2].mass() == 45);
    // [example]
}
