// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    chfl_vector3d matrix[3] = {
        {10, 0, 0},
        {0, 12, 0},
        {0, 0, 15}
    };

    CHFL_CELL* cell = chfl_cell_from_matrix(matrix);

    chfl_vector3d lengths;
    chfl_cell_lengths(cell, lengths);
    assert(lengths[0] == 10);
    assert(lengths[1] == 12);
    assert(lengths[2] == 15);

    chfl_free(cell);

    // [example]
    return 0;
}
