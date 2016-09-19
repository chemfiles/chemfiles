/* File indexes.c, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <stdio.h>
#include <stdlib.h>

#include "chemfiles.h"

int main(void) {
    CHFL_TRAJECTORY* file = chfl_trajectory_open("tests/files/xyz/helium.xyz", 'r');
    CHFL_FRAME* frame = chfl_frame(0);
    unsigned* indexes = NULL;

    if (chfl_trajectory_read(file, frame) != CHFL_SUCCESS) {/*Handle error*/}

    size_t natoms = 0;
    chfl_vector_t* positions = NULL;
    chfl_frame_positions(frame, &positions, &natoms);
    indexes = malloc(natoms * sizeof(unsigned));
    if (indexes == NULL) {/*Handle error*/}

    for (unsigned i=0; i<natoms; i++) {
        indexes[i] = (unsigned)-1;
    }

    unsigned last_index = 0;
    for (unsigned i=0; i<natoms; i++) {
        if (positions[i][0] < 5) {
            indexes[last_index] = i;
            last_index++;
        }
    }

    printf("Atoms with x < 5:\n");
    unsigned i = 0;
    while(indexes[i] != (unsigned)-1 && i < natoms) {
        printf("  - %d\n", indexes[i]);
        i++;
    }
    printf("Number of atoms: %d\n", i);

    chfl_trajectory_close(file);
    chfl_frame_free(frame);
    free(indexes);
    return EXIT_SUCCESS;
}
