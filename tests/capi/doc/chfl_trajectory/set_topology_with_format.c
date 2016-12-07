#include <chemfiles.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.nc", 'r');

    chfl_trajectory_set_topology_with_format(trajectory, "water.topo", "PDB");

    /* Reading the trajectory will use topology from water.topo using the PDB format. */

    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
