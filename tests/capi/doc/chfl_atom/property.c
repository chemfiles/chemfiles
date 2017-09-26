// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");
    CHFL_PROPERTY* property = chfl_property_double(-23);

    chfl_atom_set_property(atom, "this", property);
    chfl_property_free(property);

    property = chfl_atom_get_property(atom, "this");

    double value = 0;
    chfl_property_get_double(property, &value);
    assert(value == -23);

    chfl_property_free(property);
    chfl_atom_free(atom);
    // [example]
    return 0;
}
