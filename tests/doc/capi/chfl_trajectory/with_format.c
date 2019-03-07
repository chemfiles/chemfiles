// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_with_format("water.zeo", 'r', "XYZ");

    if (trajectory == NULL) {
        /* handle error */
    }

    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
