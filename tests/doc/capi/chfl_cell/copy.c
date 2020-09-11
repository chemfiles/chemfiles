// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10}, NULL);
    CHFL_CELL* copy = chfl_cell_copy(cell);

    if (copy == NULL) {
        /* handle error */
    }

    chfl_free(copy);
    chfl_free(cell);
    // [example]
    return 0;
}
