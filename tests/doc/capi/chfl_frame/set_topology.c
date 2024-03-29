// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main(void) {
    // [example]
    CHFL_FRAME* frame = chfl_frame();

    CHFL_TOPOLOGY* topology = chfl_topology();

    {
        // Build the topology
        CHFL_ATOM* O = chfl_atom("O");
        CHFL_ATOM* H = chfl_atom("H");

        chfl_topology_add_atom(topology, O);
        chfl_topology_add_atom(topology, H);
        chfl_topology_add_atom(topology, H);

        chfl_free(O);
        chfl_free(H);
    }

    chfl_frame_set_topology(frame, topology);

    chfl_free(topology);
    chfl_free(frame);
    // [example]
    return 0;
}
