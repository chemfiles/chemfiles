// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    // Build topology ...

    CHFL_RESIDUE* residue = chfl_residue_from_topology(topology, 3);

    if (residue == NULL) {
        /* handle error */
    }

    chfl_residue_free(residue);
    chfl_topology_free(topology);
    // [example]
    return 0;
}
