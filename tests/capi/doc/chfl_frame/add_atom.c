// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_ATOM* atom = chfl_atom("C");

    chfl_frame_add_atom(frame, atom, (chfl_vector3d){1, 2, 3}, NULL);

    chfl_atom_free(atom);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
