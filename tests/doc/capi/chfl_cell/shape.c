// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10}, NULL);

    chfl_cellshape shape;
    chfl_cell_shape(cell, &shape);
    assert(shape == CHFL_CELL_ORTHORHOMBIC);

    chfl_free(cell);
    // [example]
    return 0;
}
