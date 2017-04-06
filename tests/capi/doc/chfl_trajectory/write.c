// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdio.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    /* Add atoms to the frame */

    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.xyz", 'w');
    if (chfl_trajectory_write(trajectory, frame) != CHFL_SUCCESS) {
        /* handle error */
    }

    chfl_trajectory_close(trajectory);
    chfl_frame_free(frame);
    // [example]

    remove("water.xyz");
    return 0;
}
