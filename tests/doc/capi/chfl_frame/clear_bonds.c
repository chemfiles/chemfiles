// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_ATOM* atom = chfl_atom("C");

    chfl_frame_add_atom(frame, atom, (chfl_vector3d){0, 0, 0}, NULL);
    chfl_frame_add_atom(frame, atom, (chfl_vector3d){1, 0, 0}, NULL);
    chfl_frame_add_atom(frame, atom, (chfl_vector3d){0, 1, 0}, NULL);

    chfl_frame_add_bond(frame, 0, 1);
    chfl_frame_add_bond(frame, 2, 1);

    const CHFL_TOPOLOGY* topology = chfl_topology_from_frame(frame);
    uint64_t bonds = 0;
    chfl_topology_bonds_count(topology, &bonds);
    assert(bonds == 2);

    chfl_frame_clear_bonds(frame);
    chfl_topology_bonds_count(topology, &bonds);
    assert(bonds == 0);

    chfl_free(atom);
    chfl_free(topology);
    chfl_free(frame);
    // [example]
    return 0;
}
