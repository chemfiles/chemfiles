// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_RESIDUE* residue = chfl_residue_with_id("water", 3);

    if (residue == NULL) {
        /* handle error */
    }

    chfl_residue_free(residue);
    // [example]
    return 0;
}
