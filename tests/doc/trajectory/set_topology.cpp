// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [no-run]
    // [example]
    // A water topology
    auto topology = Topology();
    topology.add_atom(Atom("O"));
    topology.add_atom(Atom("H"));
    topology.add_atom(Atom("H"));
    topology.add_bond(0, 1);
    topology.add_bond(0, 2);

    auto trajectory = Trajectory("water.nc");
    trajectory.set_topology(topology);

    auto frame = trajectory.read();
    // The topology of the frame is now a water molecule, regardless of
    // what was defined in the trajectory.

    // One can also set the topology from a file
    trajectory.set_topology("water.pdb");

    // Or set it from a file with a specific format
    trajectory.set_topology("water.mol", "XYZ");

    // [example]
}
