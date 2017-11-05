// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto frame = Frame();
    frame.resize(3);

    auto topology = frame.topology();
    assert(topology[0].name() == "");

    // Manually constructing a topology
    topology = Topology();
    topology.add_atom(Atom("H"));
    topology.add_atom(Atom("O"));
    topology.add_atom(Atom("H"));
    topology.add_bond(0, 1);
    topology.add_bond(2, 1);

    frame.set_topology(topology);
    assert(frame.topology()[0].name() == "H");
    // [example]
}
