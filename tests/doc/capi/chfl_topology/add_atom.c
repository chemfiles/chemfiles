// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* H = chfl_atom("H");

    chfl_topology_add_atom(topology, O);
    chfl_topology_add_atom(topology, H);
    chfl_topology_add_atom(topology, H);

    chfl_free(O);
    chfl_free(H);

    chfl_free(topology);
    // [example]
    return 0;
}
