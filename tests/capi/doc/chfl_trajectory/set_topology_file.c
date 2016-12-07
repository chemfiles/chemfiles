#include <chemfiles.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.nc", 'r');

    chfl_trajectory_set_topology_file(trajectory, "water.pdb");

    /* Reading the trajectory will use topology from water.pdb */

    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
