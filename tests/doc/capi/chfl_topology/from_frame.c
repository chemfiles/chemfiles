// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    const CHFL_TOPOLOGY* topology = chfl_topology_from_frame(frame);

    if (topology == NULL) {
        /* handle error */
    }

    chfl_free(topology);
    chfl_free(frame);
    // [example]
    return 0;
}
