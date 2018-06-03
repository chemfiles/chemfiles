// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // [example]
    // Set property
    CHFL_ATOM* atom = chfl_atom("Na");
    CHFL_PROPERTY* property = chfl_property_double(-23);

    chfl_atom_set_property(atom, "this", property);
    chfl_property_free(property);
    
    // Get property
    property = chfl_atom_get_property(atom, "this");

    double value = 0;
    chfl_property_get_double(property, &value);
    assert(value == -23);

    // Get all properties
    uint64_t count = 0;
    char** names;
    chfl_atom_properties_names(atom, &count, &names);
    CHFL_PROPERTY *same_property = chfl_atom_get_property(atom, names[0]);

    value = 0;
    chfl_property_get_double(same_property, &value);
    
    assert(value == -23);
    assert(!strcmp(names[0], "this"));

    chfl_property_free(property);
    chfl_property_free(same_property);
    chfl_atom_free(atom);
    // [example]
    return 0;
}
