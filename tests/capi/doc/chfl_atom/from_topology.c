// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();
    /* Update the topology by hand, or using a file */

    CHFL_ATOM* atom = chfl_atom_from_topology(topology, 4);

    if (atom == NULL) {
        /* handle error */
    }

    chfl_atom_free(atom);
    chfl_topology_free(topology);
    // [example]
    return 0;
}
