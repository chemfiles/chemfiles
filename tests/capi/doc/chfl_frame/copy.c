// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_FRAME* copy = chfl_frame_copy(frame);

    if (copy == NULL) {
        /* handle error */
    }

    chfl_frame_free(copy);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
