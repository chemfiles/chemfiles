// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    CHFL_ATOM* atom = chfl_atom("F");
    chfl_topology_add_atom(topology, atom);
    chfl_topology_add_atom(topology, atom);
    chfl_topology_add_atom(topology, atom);
    chfl_atom_free(atom);

    uint64_t atoms = 0;
    chfl_topology_atoms_count(topology, &atoms);
    assert(atoms == 3);

    chfl_topology_free(topology);
    // [example]
    return 0;
}
