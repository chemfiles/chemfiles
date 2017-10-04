// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    CHFL_RESIDUE* residue = chfl_residue("res");
    chfl_topology_add_residue(topology, residue);
    chfl_residue_free(residue);

    uint64_t residues = 0;
    chfl_topology_residues_count(topology, &residues);
    assert(residues == 1);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
