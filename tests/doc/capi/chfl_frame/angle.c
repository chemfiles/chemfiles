// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void) {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_ATOM* atom = chfl_atom("");

    chfl_frame_add_atom(frame, atom, (chfl_vector3d){1, 0, 0}, NULL);
    chfl_frame_add_atom(frame, atom, (chfl_vector3d){0, 0, 0}, NULL);
    chfl_frame_add_atom(frame, atom, (chfl_vector3d){0, 1, 0}, NULL);

    double angle = 0;
    chfl_frame_angle(frame, 0, 1, 2, &angle);
    assert(fabs(angle - M_PI / 2) < 1e-12);

    chfl_free(atom);
    chfl_free(frame);
    // [example]
    return 0;
}
