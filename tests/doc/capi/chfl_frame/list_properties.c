// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();
    CHFL_PROPERTY* property = chfl_property_double(-23);

    chfl_frame_set_property(frame, "this", property);
    chfl_frame_set_property(frame, "that", property);
    chfl_free(property);

    uint64_t count = 0;
    chfl_frame_properties_count(frame, &count);
    assert(count == 2);

    const char* names[2] = {NULL};
    chfl_frame_list_properties(frame, names, count);

    // Properties are not ordered
    assert(strcmp(names[0], "this") == 0 || strcmp(names[0], "that") == 0);
    assert(strcmp(names[1], "this") == 0 || strcmp(names[1], "that") == 0);

    chfl_free(frame);
    // [example]
    return 0;
}
