// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    if (topology == NULL) {
        /* handle error */
    }

    chfl_topology_free(topology);
    // [example]
    return 0;
}
