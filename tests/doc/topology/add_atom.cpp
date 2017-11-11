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

    assert(topology.size() == 1);
    assert(topology[0].name() == "Zn");
    // [example]
}
