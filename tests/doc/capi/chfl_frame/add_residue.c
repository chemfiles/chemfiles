// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_RESIDUE* residue = chfl_residue("res");

    chfl_frame_add_residue(frame, residue);

    chfl_free(residue);
    chfl_free(frame);
    // [example]
    return 0;
}
