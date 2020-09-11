// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

int main() {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10}, NULL);
    chfl_cell_set_shape(cell, CHFL_CELL_TRICLINIC);

    chfl_cell_set_angles(cell, (chfl_vector3d){120, 110, 100});

    chfl_vector3d angles = {0, 0, 0};
    chfl_cell_angles(cell, angles);
    // Floating point rounding error can exist when accessing angles
    assert(fabs(angles[0] - 120) < 1e-12);
    assert(fabs(angles[1] - 110) < 1e-12);
    assert(fabs(angles[2] - 100) < 1e-12);

    chfl_free(cell);
    // [example]
    return 0;
}
