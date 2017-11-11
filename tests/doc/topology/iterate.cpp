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
    topology.add_atom(Atom("Fe"));
    topology.add_atom(Atom("Fe"));
    topology.add_atom(Atom("Fe"));

    for (Atom& atom: topology) {
        assert(atom.name() == "Fe");
    }
    // [example]
}
