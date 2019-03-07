// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.nc", 'r');

    chfl_trajectory_topology_file(trajectory, "water.pdb", NULL);
    /* Reading the trajectory will use topology from water.pdb */

    chfl_trajectory_topology_file(trajectory, "water.topo", "PDB");
    /* Reading the trajectory will use topology from water.topo using the PDB format. */

    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
