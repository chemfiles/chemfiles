// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // [example]
    CHFL_ATOM* atom = chfl_atom("Na");
    CHFL_PROPERTY* property = chfl_property_double(-23);

    chfl_atom_set_property(atom, "this", property);
    chfl_atom_set_property(atom, "that", property);
    chfl_free(property);

    uint64_t count = 0;
    chfl_atom_properties_count(atom, &count);
    assert(count == 2);

    const char* names[2] = {NULL};
    chfl_atom_list_properties(atom, names, count);

    // Properties are not ordered
    assert(strcmp(names[0], "this") == 0 || strcmp(names[0], "that") == 0);
    assert(strcmp(names[1], "this") == 0 || strcmp(names[1], "that") == 0);

    chfl_free(atom);
    // [example]
    return 0;
}
