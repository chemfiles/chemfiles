// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_RESIDUE* residue = chfl_residue("water");
    CHFL_RESIDUE* copy = chfl_residue_copy(residue);

    if (copy == NULL) {
        /* handle error */
    }

    chfl_residue_free(copy);
    chfl_residue_free(residue);
    // [example]
    return 0;
}
