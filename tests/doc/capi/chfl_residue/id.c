// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_RESIDUE* residue = chfl_residue_with_id("water", 3);

    int64_t id = 0;
    chfl_residue_id(residue, &id);
    assert(id == 3);

    chfl_free(residue);
    // [example]
    return 0;
}
