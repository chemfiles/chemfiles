// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_ATOM* atom = chfl_atom("");

    chfl_frame_add_atom(frame, atom, (chfl_vector3d){0, 0, 0}, NULL);
    chfl_frame_add_atom(frame, atom, (chfl_vector3d){1, 2, 3}, NULL);

    double distance = 0;
    chfl_frame_distance(frame, 0, 1, &distance);
    assert(distance == sqrt(14));

    chfl_atom_free(atom);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
