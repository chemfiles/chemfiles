// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_RESIDUE* residue = chfl_residue("ASP");
    CHFL_PROPERTY* property = chfl_property_double(-23);

    chfl_residue_set_property(residue, "this", property);
    chfl_free(property);

    property = chfl_residue_get_property(residue, "this");

    double value = 0;
    chfl_property_get_double(property, &value);
    assert(value == -23);

    chfl_free(property);
    chfl_free(residue);
    // [example]
    return 0;
}
