// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10});

    chfl_vector3d angles = {0, 0, 0};
    chfl_cell_angles(cell, angles);
    assert(angles[0] == 90);
    assert(angles[1] == 90);
    assert(angles[2] == 90);

    chfl_cell_free(cell);
    // [example]
    return 0;
}
