/* File convert.cpp, example for the Chemharp library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include "Chemharp.hpp"

int main() {
    harp::Trajectory input("water.xyz");
    harp::Frame frame{};
    harp::Topology water_topology{};
    // Orthorombic UnitCell with lengths of 20, 15 and 35 A
    harp::UnitCell cell(20, 15, 35);

    // Create Atoms
    harp::Atom O("O");
    harp::Atom H("H");

    // Fill the topology with one water molecule
    water_topology.append(O);
    water_topology.append(H);
    water_topology.append(H);
    water_topology.add_bond(0, 1);
    water_topology.add_bond(0, 2);

    harp::Trajectory output("water.pdb", "w");

    while (!input.done()) {
        input >> frame;
        // Set the frame cell and topology
        frame.cell(cell);
        frame.topology(water_topology);
        // Write the frame to the output file, using PDB format
        output << frame;
    }

    return 0;
}
