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
    topology.add_atom(Atom("Rd"));
    assert(topology.size() == 3);

    assert(topology[1].name() == "Fe");
    topology.remove(0);

    // atomic indexes are shifted by remove
    assert(topology[1].name() == "Rd");
    // [example]
}
