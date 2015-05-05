/* File rmsd.c, example for the Chemharp library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "chemharp.h"

int main() {
    CHRP_TRAJECTORY* traj = chrp_open("filename.nc", "r");
    CHRP_FRAME* frame = chrp_frame(0);
    float (*positions)[3] = NULL;
    double* distances = NULL;

    if(traj == NULL || frame == NULL)
        goto error;

    size_t nsteps = 0;
    chrp_trajectory_nsteps(traj, &nsteps);

    distances = (double*)malloc(sizeof(double)*nsteps);
    if (distances == NULL)
        goto error;

    // Accumulate the distances to the origin of the 10th atom throughtout the
    // trajectory
    for (size_t i=0; i<nsteps; i++) {
        if(!chrp_trajectory_read(traj, frame))
            goto error;

        // Only allocate on the first iteration. This assume a constant number
        // of particles
        size_t natoms = 0;
        if (i == 0) {
            chrp_frame_size(frame, &natoms);
            positions = (float(*)[3])malloc(sizeof(float[natoms][3]));
            if (positions == NULL)
                goto error;
        }

        // Position of the 10th atom
        chrp_frame_positions(frame, positions, natoms);
        double distance = sqrt(positions[9][0]*positions[9][0] +
                               positions[9][1]*positions[9][1] +
                               positions[9][2]*positions[9][2]);
        distances[i] = distance;
    }

    double mean = 0;
    for (size_t i=0; i<nsteps; i++) {
        mean += distances[i];
    }
    mean /= nsteps;

    double rmsd = 0.0;
    for (size_t i=0; i<nsteps; i++) {
        rmsd += (mean - distances[i])*(mean - distances[i]);
    }
    rmsd /= nsteps;
    rmsd = sqrt(rmsd);

    printf("Root-mean square displacement is: %f", rmsd);

    // Free the memory
    chrp_trajectory_close(traj);
    chrp_frame_free(frame);
    free(distances);
    free(positions);
    return 0;

error:
    printf("Error, cleaning up …\n");
    chrp_trajectory_close(traj);
    chrp_frame_free(frame);
    free(distances);
    free(positions);
    return 1;
}
