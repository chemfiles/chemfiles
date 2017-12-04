/* This file is an example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <chemfiles.hpp>

int main() {
    chemfiles::Topology topology;
    topology.add_atom(chemfiles::Atom("H"));
    topology.add_atom(chemfiles::Atom("O"));
    topology.add_atom(chemfiles::Atom("H"));

    topology.add_bond(0, 1);
    topology.add_bond(2, 1);

    chemfiles::Frame frame(topology);
    auto positions = frame.positions();

    positions[0] = chemfiles::Vector3D(1, 0, 0);
    positions[1] = chemfiles::Vector3D(0, 0, 0);
    positions[2] = chemfiles::Vector3D(0, 1, 0);

    frame.add_atom(chemfiles::Atom("O"), {5, 0, 0});
    frame.add_atom(chemfiles::Atom("C"), {6, 0, 0});
    frame.add_atom(chemfiles::Atom("O"), {7, 0, 0});
    frame.add_bond(3, 4);
    frame.add_bond(4, 5);

    frame.set_cell(chemfiles::UnitCell(10, 10, 10));

    auto trajectory = chemfiles::Trajectory("water-co2.pdb", 'w');
    trajectory.write(frame);

    return 0;
}
