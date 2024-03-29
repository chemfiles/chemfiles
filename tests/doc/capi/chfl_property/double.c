// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>

int main(void) {
    // [example]
    CHFL_PROPERTY* property = chfl_property_double(256);

    double value = 0;
    chfl_property_get_double(property, &value);
    assert(value == 256);

    chfl_free(property);
    // [example]
    return 0;
}
