/* File indexes.c, example for the Chemharp library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <stdio.h>
#include <stdlib.h>

#include "chemharp.h"

int main() {
    CHRP_TRAJECTORY* input = chrp_open("water.xyz", "r");
    CHRP_TRAJECTORY* output = chrp_open("water.pdb", "w");
    CHRP_FRAME* frame = chrp_frame(0);
    CHRP_TOPOLOGY* water_topology = chrp_topology();
    // Orthorombic UnitCell with lengths of 20, 15 and 35 A
    CHRP_CELL* cell = chrp_cell(20, 15, 35, 90, 90, 90);

    // Create Atoms
    CHRP_ATOM* O = chrp_atom("O");
    CHRP_ATOM* H = chrp_atom("H");

    if (input == NULL || frame == NULL || water_topology == NULL ||
        cell == NULL || O == NULL || H == NULL || output == NULL)
            goto error;

    // Fill the topology with one water molecule
    chrp_topology_append(water_topology, O);
    chrp_topology_append(water_topology, H);
    chrp_topology_append(water_topology, H);
    chrp_topology_add_bond(water_topology, 0, 1);
    chrp_topology_add_bond(water_topology, 0, 2);

    size_t nsteps;
    chrp_trajectory_nsteps(input, &nsteps);

    for (size_t i=0; i<nsteps; i++) {
        chrp_trajectory_read(input, frame);
        // Set the frame cell and topology
        chrp_frame_cell_set(frame, cell);
        chrp_frame_topology_set(frame, water_topology);
        // Write the frame to the output file, using PDB format
        chrp_trajectory_write(output, frame);
    }

    chrp_trajectory_close(input);
    chrp_trajectory_close(output);
    chrp_frame_free(frame);
    chrp_cell_free(cell);
    chrp_topology_free(water_topology);
    chrp_atom_free(O);
    chrp_atom_free(H);
    return 0;

error:
    printf("Error, cleaning up …\n");
    chrp_trajectory_close(input);
    chrp_trajectory_close(output);
    chrp_frame_free(frame);
    chrp_cell_free(cell);
    chrp_topology_free(water_topology);
    chrp_atom_free(O);
    chrp_atom_free(H);
    return 1;
}
