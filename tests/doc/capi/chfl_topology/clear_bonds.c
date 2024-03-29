// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();
    CHFL_ATOM* atom = chfl_atom("C");

    chfl_topology_add_atom(topology, atom);
    chfl_topology_add_atom(topology, atom);
    chfl_topology_add_atom(topology, atom);

    chfl_topology_add_bond(topology, 0, 1);
    chfl_topology_add_bond(topology, 2, 1);

    uint64_t bonds = 0;
    chfl_topology_bonds_count(topology, &bonds);
    assert(bonds == 2);

    chfl_topology_clear_bonds(topology);
    chfl_topology_bonds_count(topology, &bonds);
    assert(bonds == 0);

    chfl_free(atom);
    chfl_free(topology);
    // [example]
    return 0;
}
