// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main(void) {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    // Build topology ...

    const CHFL_RESIDUE* residue = chfl_residue_for_atom(topology, 3);

    if (residue == NULL) {
        /* handle error */
    }

    chfl_free(residue);
    chfl_free(topology);
    // [example]
    return 0;
}
