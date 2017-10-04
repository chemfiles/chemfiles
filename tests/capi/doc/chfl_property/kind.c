// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_PROPERTY* property = chfl_property_double(256);

    chfl_property_kind kind;
    chfl_property_get_kind(property, &kind);
    assert(kind == CHFL_PROPERTY_DOUBLE);

    chfl_property_free(property);
    // [example]
    return 0;
}
