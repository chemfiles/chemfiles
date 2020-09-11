// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.xyz", 'r');
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){22, 22, 34}, NULL);

    chfl_trajectory_set_cell(trajectory, cell);

    /* Reading from the trajectory use the cell */

    chfl_free(cell);
    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
