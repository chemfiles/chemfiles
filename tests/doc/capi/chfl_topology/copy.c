// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main(void) {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    CHFL_ATOM* atom = chfl_atom("F");
    chfl_topology_add_atom(topology, atom);
    chfl_topology_add_atom(topology, atom);
    chfl_topology_add_atom(topology, atom);
    chfl_free(atom);

    CHFL_TOPOLOGY* copy = chfl_topology_copy(topology);

    if (copy == NULL) {
        /* handle error */
    }

    chfl_free(copy);
    chfl_free(topology);
    // [example]
    return 0;
}
