// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_CELL* cell = chfl_cell_triclinic((chfl_vector3d){10, 10, 10}, (chfl_vector3d){90, 90, 90});

    chfl_cell_set_angles(cell, (chfl_vector3d){120, 110, 100});

    chfl_vector3d angles = {0, 0, 0};
    chfl_cell_angles(cell, angles);
    assert(angles[0] == 120);
    assert(angles[1] == 110);
    assert(angles[2] == 100);

    chfl_cell_free(cell);
    // [example]
    return 0;
}
