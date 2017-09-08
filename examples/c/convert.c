/* File indexes.c, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <stdio.h>
#include <stdlib.h>

#include "chemfiles.h"

int main(void) {
    CHFL_TRAJECTORY* input = chfl_trajectory_open("water.xyz", 'r');

    // Set the unit cell to an orthorombic cell with lengths of 20, 15 and 35 A
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){20, 15, 35});
    chfl_trajectory_set_cell(input, cell);
    chfl_cell_free(cell);

    // Create a water molecule topology
    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* H = chfl_atom("H");
    CHFL_TOPOLOGY* water = chfl_topology();
    chfl_topology_add_atom(water, O);
    chfl_topology_add_atom(water, H);
    chfl_topology_add_atom(water, H);
    chfl_topology_add_bond(water, 0, 1);
    chfl_topology_add_bond(water, 0, 2);

    chfl_trajectory_set_topology(input, water);

    chfl_topology_free(water);
    chfl_atom_free(O);
    chfl_atom_free(H);

    uint64_t nsteps = 0;
    chfl_trajectory_nsteps(input, &nsteps);

    CHFL_TRAJECTORY* output = chfl_trajectory_open("water.pdb", 'w');
    CHFL_FRAME* frame = chfl_frame();
    for (uint64_t i=0; i<nsteps; i++) {
        // The unit cell and the topology are automatically set when reading a
        // frame.
        chfl_trajectory_read(input, frame);
        chfl_trajectory_write(output, frame);
    }

    chfl_trajectory_close(input);
    chfl_trajectory_close(output);
    chfl_frame_free(frame);
    return 0;
}
