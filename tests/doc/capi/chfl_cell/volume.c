// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10}, NULL);

    double volume = 0;
    chfl_cell_volume(cell, &volume);
    assert(volume == 1000);

    chfl_free(cell);
    // [example]
    return 0;
}
