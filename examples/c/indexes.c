/* This file is an example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <chemfiles.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    CHFL_TRAJECTORY* file = chfl_trajectory_open("filename.xyz", 'r');
    CHFL_FRAME* frame = chfl_frame();
    chfl_trajectory_read(file, frame);

    uint64_t natoms = 0;
    chfl_vector3d* positions = NULL;
    chfl_frame_positions(frame, &positions, &natoms);

    size_t* less_than_five = malloc((size_t)natoms * sizeof(size_t));

    size_t matched = 0;
    for (uint64_t i=0; i<natoms; i++) {
        if (positions[i][0] < 5) {
            less_than_five[matched] = (size_t)i;
            matched++;
        }
    }

    printf("Atoms with x < 5:\n");
    for (size_t i=0; i<matched; i++) {
        printf("  - %zu", less_than_five[i]);
    }

    free(less_than_five);
    chfl_free(frame);
    chfl_trajectory_close(file);

    return 0;
}
