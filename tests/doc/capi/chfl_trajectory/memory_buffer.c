// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    /* Add atoms to the frame */

    CHFL_TRAJECTORY* trajectory = chfl_trajectory_memory_writer("XYZ");
    if (chfl_trajectory_write(trajectory, frame) != CHFL_SUCCESS) {
        /* handle error */
    }

    const char* result = NULL;
    uint64_t size_of_result;
    chfl_trajectory_memory_buffer(trajectory, &result, &size_of_result);

    chfl_trajectory_close(trajectory);
    chfl_free(frame);
    // [example]

    return 0;
}
