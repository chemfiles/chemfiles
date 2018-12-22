// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example] [no-run]
    CHFL_TOPOLOGY* topology = chfl_topology();

    // Build the topology ...

    const CHFL_RESIDUE* first = chfl_residue_from_topology(topology, 0);
    const CHFL_RESIDUE* second = chfl_residue_from_topology(topology, 1);

    bool linked = false;
    chfl_topology_residues_linked(topology, first, second, &linked);

    chfl_free(first);
    chfl_free(second);

    chfl_free(topology);
    // [example]
    return 0;
}
