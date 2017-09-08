// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    chfl_frame_add_velocities(frame);

    chfl_vector3d* velocities = NULL;
    uint64_t natoms = 0;
    chfl_frame_positions(frame, &velocities, &natoms);

    for (uint64_t i=0; i<natoms; i++) {
        // use velocities[i] here
    }

    chfl_frame_free(frame);
    // [example]
    return 0;
}
