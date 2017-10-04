// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_RESIDUE* residue = chfl_residue("water");

    chfl_residue_add_atom(residue, 0);
    chfl_residue_add_atom(residue, 32);
    chfl_residue_add_atom(residue, 28);

    bool contained = false;
    chfl_residue_contains(residue, 32, &contained);
    assert(contained == true);

    chfl_residue_contains(residue, 11, &contained);
    assert(contained == false);

    chfl_residue_free(residue);
    // [example]
    return 0;
}
