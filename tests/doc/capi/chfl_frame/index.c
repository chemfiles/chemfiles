// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_FRAME* frame = chfl_frame();

    uint64_t index = 3;
    chfl_frame_index(frame, &index);
    assert(index == 0);

    chfl_free(frame);
    // [example]
    return 0;
}
