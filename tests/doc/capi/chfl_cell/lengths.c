// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 11, 12}, NULL);

    chfl_vector3d lengths = {0, 0, 0};
    chfl_cell_lengths(cell, lengths);
    assert(lengths[0] == 10);
    assert(lengths[1] == 11);
    assert(lengths[2] == 12);

    chfl_free(cell);
    // [example]
    return 0;
}
