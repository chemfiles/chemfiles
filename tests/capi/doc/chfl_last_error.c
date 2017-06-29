// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <string.h>
#include <assert.h>

int main() {
    // [example]
    // Generate an error
    chfl_trajectory_open("noformat", 'r');

    const char* error = chfl_last_error();
    assert(strcmp(error, "Can not find a format associated with the '' extension.") == 0);
    // [example]
    return 0;
}
