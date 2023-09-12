// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main(void) {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    if (topology == NULL) {
        /* handle error */
    }

    chfl_free(topology);
    // [example]
    return 0;
}
