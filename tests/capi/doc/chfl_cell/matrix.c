// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 11, 12});

    chfl_vector3d matrix[3];
    chfl_cell_matrix(cell, matrix);
    assert(matrix[0][0] == 10);
    assert(matrix[1][1] == 11);
    assert(matrix[2][2] == 12);

    // Out of diagonal terms are zero
    assert(matrix[2][1] == 0);

    chfl_cell_free(cell);
    // [example]
    return 0;
}
