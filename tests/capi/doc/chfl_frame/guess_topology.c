// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    // Building a frame containing a Cl2 molecule
    CHFL_FRAME* frame = chfl_frame();

    CHFL_ATOM* Cl = chfl_atom("Cl");
    chfl_frame_add_atom(frame, Cl, (chfl_vector3d){0, 0, 0}, NULL);
    chfl_frame_add_atom(frame, Cl, (chfl_vector3d){2, 0, 0}, NULL);
    chfl_atom_free(Cl);

    // Check that the topology does not contain any bond
    CHFL_TOPOLOGY* topology = chfl_topology_from_frame(frame);
    uint64_t bonds = 0;
    chfl_topology_bonds_count(topology, &bonds);
    assert(bonds == 0);
    chfl_topology_free(topology);

    chfl_frame_guess_topology(frame);

    // Get a copie of the new topology
    topology = chfl_topology_from_frame(frame);
    chfl_topology_bonds_count(topology, &bonds);
    assert(bonds == 1);

    chfl_topology_free(topology);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
