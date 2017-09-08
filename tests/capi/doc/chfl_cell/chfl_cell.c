// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10});

    if (cell == NULL) {
        /* handle error */
    }

    chfl_cell_free(cell);
    // [example]
    return 0;
}
