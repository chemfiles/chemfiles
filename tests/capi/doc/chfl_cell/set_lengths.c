// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10});

    chfl_cell_set_lengths(cell, (chfl_vector3d){42, 8, 3});

    chfl_vector3d lengths = {0, 0, 0};
    chfl_cell_lengths(cell, lengths);
    assert(lengths[0] == 42);
    assert(lengths[1] == 8);
    assert(lengths[2] == 3);

    chfl_cell_free(cell);
    // [example]
    return 0;
}
