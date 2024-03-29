// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main(void) {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");

    if (atom == NULL) {
        /* handle error */
    }

    chfl_free(atom);
    // [example]
    return 0;
}
