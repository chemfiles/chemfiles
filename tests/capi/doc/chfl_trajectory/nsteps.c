// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.nc", 'r');

    uint64_t nsteps = 0;
    chfl_trajectory_nsteps(trajectory, &nsteps);

    /* Read all steps in the trajectory */
    CHFL_FRAME* frame = chfl_frame();
    for (uint64_t i=0; i<nsteps; i++) {
        chfl_trajectory_read(trajectory, frame);
        /* Do stuff with the frame */
    }

    chfl_frame_free(frame);
    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
