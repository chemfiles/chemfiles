// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <chemfiles.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.xyz", 'r');

    char path[256] = {0};
    chfl_trajectory_path(trajectory, path, sizeof(path));
    assert(strcmp(path, "water.xyz") == 0);

    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
