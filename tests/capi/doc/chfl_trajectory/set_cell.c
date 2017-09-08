// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.xyz", 'r');
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){22, 22, 34});

    chfl_trajectory_set_cell(trajectory, cell);

    /* Reading from the trajectory use the cell */

    chfl_cell_free(cell);
    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
