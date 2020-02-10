// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // [example]
    const char* memory_buffer = "c1ccccc1\nc1ccco1\nc1ccccn1\n";
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_memory_reader(memory_buffer, strlen(memory_buffer), "SMI");

    if (trajectory == NULL) {
        /* handle error */
    }

    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
