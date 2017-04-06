// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();

    chfl_frame_resize(frame, 55);

    uint64_t atoms = 0;
    chfl_frame_atoms_count(frame, &atoms);
    assert(atoms == 55);

    chfl_frame_free(frame);
    // [example]
    return 0;
}
