// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    CHFL_ATOM* atom = chfl_atom("F");
    for (size_t i=0; i<5; i++) {
        chfl_topology_add_atom(topology, atom);
    }
    chfl_atom_free(atom);

    chfl_topology_add_bond(topology, 3, 0);
    chfl_topology_add_bond(topology, 3, 1);
    chfl_topology_add_bond(topology, 3, 2);

    uint64_t impropers[1][4] = {{0}};
    chfl_topology_impropers(topology, impropers, 1);
    assert(impropers[0][0] == 0);
    assert(impropers[0][1] == 3);
    assert(impropers[0][2] == 1);
    assert(impropers[0][3] == 2);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
