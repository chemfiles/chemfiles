// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    chfl_topology_add_bond(topology, 0, 1);
    chfl_topology_add_bond(topology, 2, 3);

    uint64_t bonds = 0;
    chfl_topology_bonds_count(topology, &bonds);
    assert(bonds == 2);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
