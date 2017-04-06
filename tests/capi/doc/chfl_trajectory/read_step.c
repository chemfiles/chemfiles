// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.xyz", 'r');
    CHFL_FRAME* frame = chfl_frame();

    chfl_trajectory_read_step(trajectory, 42, frame);

    /* We can use the 42nd frame here */

    chfl_frame_free(frame);
    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
