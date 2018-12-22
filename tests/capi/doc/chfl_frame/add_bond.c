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
    chfl_frame_add_atom(frame, atom, (chfl_vector3d){0, 0, 1.2}, NULL);
    chfl_frame_add_atom(frame, atom, (chfl_vector3d){0, 0, -1.0}, NULL);
    chfl_frame_add_bond(frame, 0, 1);
    chfl_frame_bond_with_order(frame, 0, 2, CHFL_BOND_DOUBLE);

    chfl_free(atom);
    chfl_free(frame);
    // [example]
    return 0;
}
