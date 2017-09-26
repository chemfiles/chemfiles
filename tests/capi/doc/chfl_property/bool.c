// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_PROPERTY* property = chfl_property_bool(true);

    bool value = false;
    chfl_property_get_bool(property, &value);
    assert(value == true);

    chfl_property_free(property);
    // [example]
    return 0;
}
