// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();

    bool velocities = true;
    chfl_frame_has_velocities(frame, &velocities);
    assert(velocities == false);

    chfl_frame_free(frame);
    // [example]
    return 0;
}
