// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    /* Update the frame, or read it from a file */
    chfl_frame_resize(frame, 5);

    CHFL_ATOM* atom = chfl_atom_from_frame(frame, 4);

    if (atom == NULL) {
        /* handle error */
    }

    chfl_atom_free(atom);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
