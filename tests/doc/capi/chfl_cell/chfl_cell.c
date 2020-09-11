// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    /* Orthorhombic cell */
    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10}, NULL);

    if (cell == NULL) { /* handle error */ }

    chfl_free(cell);

    /* Triclinic cell */
    cell = chfl_cell((chfl_vector3d){10, 10, 10}, (chfl_vector3d){92, 88, 100});

    if (cell == NULL) { /* handle error */ }

    chfl_free(cell);
    // [example]
    return 0;
}
