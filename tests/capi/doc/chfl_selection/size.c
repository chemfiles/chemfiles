// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_SELECTION* selection = chfl_selection("pairs: name(#1) O and name(#2) H");

    uint64_t size = 0;
    chfl_selection_size(selection, &size);
    assert(size == 2);

    chfl_selection_free(selection);
    // [example]
    return 0;
}
