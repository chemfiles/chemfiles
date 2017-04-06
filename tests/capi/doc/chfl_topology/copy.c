// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    CHFL_ATOM* atom = chfl_atom("F");
    chfl_topology_add_atom(topology, atom);
    chfl_topology_add_atom(topology, atom);
    chfl_topology_add_atom(topology, atom);
    chfl_atom_free(atom);

    CHFL_TOPOLOGY* copy = chfl_topology_copy(topology);

    if (copy == NULL) {
        /* handle error */
    }

    chfl_topology_free(copy);
    chfl_topology_free(topology);
    // [example]
    return 0;
}
