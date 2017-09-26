// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <string.h>

int main() {
    // [example]
    CHFL_PROPERTY* property = chfl_property_string("a great property");

    char value[32];
    chfl_property_get_string(property, value, sizeof(value));
    assert(strcmp(value, "a great property") == 0);

    chfl_property_free(property);
    // [example]
    return 0;
}
