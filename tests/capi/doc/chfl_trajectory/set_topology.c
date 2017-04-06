// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>

int main() {
    // [example] [no-run]
    CHFL_TOPOLOGY* topology = chfl_topology();
    /* Build the topology by hand or by reading a file */

    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.xyz", 'r');
    chfl_trajectory_set_topology(trajectory, topology);

    /* Reading from the trajectory use the topology we built */

    chfl_topology_free(topology);
    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
