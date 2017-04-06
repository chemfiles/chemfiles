// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <string.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");

    char name[32] = {0};
    chfl_atom_full_name(atom, name, sizeof(name));
    assert(strcmp(name, "Sodium") == 0);

    chfl_atom_free(atom);
    // [example]
    return 0;
}
