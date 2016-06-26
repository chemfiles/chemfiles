/* File rmsd.c, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "chemfiles.h"

int main(void) {
    CHFL_TRAJECTORY* file = chfl_trajectory_open("filename.nc", 'r');
    size_t nsteps = 0;
    chfl_trajectory_nsteps(file, &nsteps);

    double* distances = malloc(sizeof(double) * nsteps);
    if (distances == NULL) {/*Handle error*/}

    CHFL_FRAME* frame = chfl_frame(0);
    // Accumulate the distances to the origin of the 10th atom throughtout the
    // trajectory
    for (size_t i=0; i<nsteps; i++) {
        if(!chfl_trajectory_read(file, frame)) {/*Handle error*/}

        size_t natoms = 0;
        float (*positions)[3] = NULL;

        // Get a pointer to the positions in `positions`. The array `positions`
        // contains natoms entries.
        chfl_frame_positions(frame, &positions, &natoms);
        if (natoms < 10) {
            printf("Not enough atoms in the frame\n");
            continue;
        }
        // Get the distance to the origin of the 10th atom
        double distance = sqrt(positions[9][0] * positions[9][0] +
                               positions[9][1] * positions[9][1] +
                               positions[9][2]*positions[9][2]);
        distances[i] = distance;
    }

    double mean = 0;
    for (size_t i=0; i<nsteps; i++) {
        mean += distances[i];
    }
    mean /= (double)nsteps;

    double rmsd = 0.0;
    for (size_t i=0; i<nsteps; i++) {
        rmsd += (mean - distances[i])*(mean - distances[i]);
    }
    rmsd /= (double)nsteps;
    rmsd = sqrt(rmsd);

    printf("Root-mean square displacement is: %f", rmsd);

    chfl_trajectory_close(file);
    chfl_frame_free(frame);
    free(distances);
    return 0;
}
