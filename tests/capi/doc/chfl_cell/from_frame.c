// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_CELL* cell = chfl_cell_from_frame(frame);

    if (cell == NULL) {
        /* handle error */
    }

    chfl_cell_free(cell);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
