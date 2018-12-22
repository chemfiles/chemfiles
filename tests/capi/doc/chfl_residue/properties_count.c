// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_RESIDUE* residue = chfl_residue("ALA");
    CHFL_PROPERTY* property = chfl_property_double(-23);

    chfl_residue_set_property(residue, "this", property);
    chfl_residue_set_property(residue, "that", property);
    chfl_free(property);

    uint64_t count = 0;
    chfl_residue_properties_count(residue, &count);
    assert(count == 2);

    chfl_free(residue);
    // [example]
    return 0;
}
