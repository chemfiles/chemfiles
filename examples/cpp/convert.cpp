/* File convert.cpp, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include "chemfiles.hpp"

int main() {
    // Open the input file for reading
    chemfiles::Trajectory input("water.xyz");

    // Set the unit cell to an orthorombic cell with lengths of 20, 15 and 35 A
    input.set_cell(chemfiles::UnitCell(20, 15, 35));

    // Create a water molecule topology
    chemfiles::Topology water;
    water.add_atom(chemfiles::Atom("O"));
    water.add_atom(chemfiles::Atom("H"));
    water.add_atom(chemfiles::Atom("H"));
    water.add_bond(0, 1);
    water.add_bond(0, 2);
    input.set_topology(water);

    // Write to the output file using PDB format
    chemfiles::Trajectory output("water.pdb", 'w');
    while (!input.done()) {
        // The UnitCell and the Topology are automatically set when reading a
        // frame to the specified cell and topology.
        auto frame = input.read();
        output.write(frame);
    }

    return 0;
}
