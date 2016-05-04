/* File indexes.c, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <stdio.h>
#include <stdlib.h>

#include "chemfiles.h"

int main() {
    CHFL_TRAJECTORY* traj = chfl_trajectory_open("tests/files/xyz/helium.xyz", 'r');
    CHFL_FRAME* frame = chfl_frame(0);

    if (traj == NULL)
        goto error;

    if (chfl_trajectory_read(traj, frame))
        goto error;

    size_t natoms = 0;
    float (*positions)[3] = NULL;
    chfl_frame_positions(frame, &positions, &natoms);
    unsigned* indexes = (unsigned*)malloc(natoms*sizeof(unsigned));
    if (indexes == NULL)
        goto error;

    for (int i=0; i<natoms; i++) {
        indexes[i] = (unsigned)-1;
    }

    unsigned last_index = 0;
    for (int i=0; i<natoms; i++) {
        if (positions[i][0] < 5) {
            indexes[last_index] = i;
            last_index++;
        }
    }

    printf("Atoms with x < 5:\n");
    int i = 0;
    while(indexes[i] != (unsigned)-1 && i < natoms) {
        printf("  - %d\n", indexes[i]);
        i++;
    }
    printf("Number of atoms: %d\n", i);

    chfl_trajectory_close(traj);
    chfl_frame_free(frame);
    free(positions);
    return 0;

error:
    printf("Error, cleaning up …\n");
    chfl_trajectory_close(traj);
    chfl_frame_free(frame);
    free(positions);
    return 1;
}
