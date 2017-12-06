/* This file is an example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <chemfiles.h>
#include <stdlib.h>

int main() {
    CHFL_TOPOLOGY* topology = chfl_topology();
    CHFL_ATOM* H = chfl_atom("H");
    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* C = chfl_atom("C");

    chfl_topology_add_atom(topology, H);
    chfl_topology_add_atom(topology, O);
    chfl_topology_add_atom(topology, H);

    chfl_topology_add_bond(topology, 0, 1);
    chfl_topology_add_bond(topology, 2, 1);

    CHFL_FRAME* frame = chfl_frame();
    chfl_frame_resize(frame, 3);
    chfl_frame_set_topology(frame, topology);
    chfl_topology_free(topology);

    chfl_vector3d* positions = NULL;
    uint64_t natoms = 0;
    chfl_frame_positions(frame, &positions, &natoms);

    positions[0][0] = 1.0; positions[0][1] = 0.0; positions[0][2] = 0.0;
    positions[1][0] = 0.0; positions[1][1] = 0.0; positions[1][2] = 0.0;
    positions[2][0] = 0.0; positions[2][1] = 1.0; positions[2][2] = 0.0;

    chfl_frame_add_atom(frame, O, (chfl_vector3d){5, 0, 0}, NULL);
    chfl_frame_add_atom(frame, C, (chfl_vector3d){6, 0, 0}, NULL);
    chfl_frame_add_atom(frame, O, (chfl_vector3d){7, 0, 0}, NULL);
    chfl_frame_add_bond(frame, 3, 4);
    chfl_frame_add_bond(frame, 4, 5);

    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10});
    chfl_frame_set_cell(frame, cell);
    chfl_cell_free(cell);

    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water-co2.pdb", 'w');
    chfl_trajectory_write(trajectory, frame);
    chfl_trajectory_close(trajectory);

    chfl_frame_free(frame);
    chfl_atom_free(H);
    chfl_atom_free(O);
    chfl_atom_free(C);

    return 0;
}
