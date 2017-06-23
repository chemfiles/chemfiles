// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10});

    chfl_vector3d position = {4, 12, -18};
    chfl_cell_wrap(cell, position);

    assert(position[0] == 4);
    assert(position[1] == 2);
    assert(position[2] == 2);

    chfl_cell_free(cell);
    // [example]
    return 0;
}
