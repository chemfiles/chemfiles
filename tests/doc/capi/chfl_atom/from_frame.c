// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main(void) {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    /* Update the frame, or read it from a file */
    chfl_frame_resize(frame, 5);

    CHFL_ATOM* atom = chfl_atom_from_frame(frame, 4);

    if (atom == NULL) {
        /* handle error */
    }

    chfl_free(atom);
    chfl_free(frame);
    // [example]
    return 0;
}
