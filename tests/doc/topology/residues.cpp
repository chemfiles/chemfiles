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
    topology.add_residue(Residue("first"));
    topology.add_residue(Residue("second"));

    auto residues = topology.residues();
    assert(residues.size() == 2);
    assert(residues[0].name() == "first");
    assert(residues[1].name() == "second");
    // [example]
}
