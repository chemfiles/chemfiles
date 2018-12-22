// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");
    CHFL_ATOM* copy = chfl_atom_copy(atom);

    if (copy == NULL) {
        /* handle error */
    }

    chfl_free(copy);
    chfl_free(atom);
    // [example]
    return 0;
}
