// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_PROPERTY* property = chfl_property_double(-23);

    chfl_frame_set_property(frame, "this", property);
    chfl_property_free(property);

    property = chfl_frame_get_property(frame, "this");

    double value = 0;
    chfl_property_get_double(property, &value);
    assert(value == -23);

    chfl_property_free(property);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
