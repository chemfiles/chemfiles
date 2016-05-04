/* File indexes.c, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <stdio.h>
#include <stdlib.h>

#include "chemfiles.h"

int main() {
    CHFL_TRAJECTORY* input = chfl_trajectory_open("water.xyz", 'r');
    CHFL_TRAJECTORY* output = chfl_trajectory_open("water.pdb", 'w');
    CHFL_FRAME* frame = chfl_frame(0);
    CHFL_TOPOLOGY* water_topology = chfl_topology();
    // Orthorombic UnitCell with lengths of 20, 15 and 35 A
    CHFL_CELL* cell = chfl_cell(20, 15, 35);

    // Create Atoms
    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* H = chfl_atom("H");

    if (input == NULL || frame == NULL || water_topology == NULL ||
        cell == NULL || O == NULL || H == NULL || output == NULL)
            goto error;

    // Fill the topology with one water molecule
    chfl_topology_append(water_topology, O);
    chfl_topology_append(water_topology, H);
    chfl_topology_append(water_topology, H);
    chfl_topology_add_bond(water_topology, 0, 1);
    chfl_topology_add_bond(water_topology, 0, 2);

    size_t nsteps;
    chfl_trajectory_nsteps(input, &nsteps);

    for (size_t i=0; i<nsteps; i++) {
        chfl_trajectory_read(input, frame);
        // Set the frame cell and topology
        chfl_frame_set_cell(frame, cell);
        chfl_frame_set_topology(frame, water_topology);
        // Write the frame to the output file, using PDB format
        chfl_trajectory_write(output, frame);
    }

    chfl_trajectory_close(input);
    chfl_trajectory_close(output);
    chfl_frame_free(frame);
    chfl_cell_free(cell);
    chfl_topology_free(water_topology);
    chfl_atom_free(O);
    chfl_atom_free(H);
    return 0;

error:
    printf("Error, cleaning up …\n");
    chfl_trajectory_close(input);
    chfl_trajectory_close(output);
    chfl_frame_free(frame);
    chfl_cell_free(cell);
    chfl_topology_free(water_topology);
    chfl_atom_free(O);
    chfl_atom_free(H);
    return 1;
}
