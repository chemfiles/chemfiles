// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main(void) {
    // [example]
    CHFL_SELECTION* selection = chfl_selection("name O");
    CHFL_SELECTION* copy = chfl_selection_copy(selection);

    if (copy == NULL) {
        /* handle error */
    }

    chfl_free(copy);
    chfl_free(selection);
    // [example]
    return 0;
}
