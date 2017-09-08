// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");

    uint64_t number = 0;
    chfl_atom_atomic_number(atom, &number);
    assert(number == 11);

    chfl_atom_free(atom);
    // [example]
    return 0;
}
