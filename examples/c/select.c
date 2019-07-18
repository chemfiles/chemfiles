/* This file is an example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <chemfiles.h>
#include <stdlib.h>

static int compare_matches(const void* lhs, const void* rhs);

int main(void) {
    CHFL_TRAJECTORY* input = chfl_trajectory_open("input.arc", 'r');
    CHFL_TRAJECTORY* output = chfl_trajectory_open("output.pdb", 'w');
    CHFL_FRAME* frame = chfl_frame();
    CHFL_SELECTION* selection = chfl_selection("name Zn or name N");

    uint64_t nsteps = 0;
    chfl_trajectory_nsteps(input, &nsteps);

    for (size_t step=0; step<nsteps; step++) {
        chfl_trajectory_read(input, frame);

        uint64_t n_matches = 0;
        chfl_selection_evaluate(selection, frame, &n_matches);
        chfl_match* matches = malloc((size_t)n_matches * sizeof(chfl_match));
        chfl_selection_matches(selection, matches, n_matches);

        uint64_t* to_remove = malloc((size_t)n_matches * sizeof(uint64_t));
        for (size_t i = 0; i<n_matches; i++) {
            to_remove[i] = matches[i].atoms[0];
        }

        qsort(to_remove, (size_t)n_matches, sizeof(uint64_t), compare_matches);
        for (size_t i = 0; i<n_matches; i++) {
            chfl_frame_remove(frame, to_remove[i]);
        }

        chfl_trajectory_write(output, frame);

        free(to_remove);
        free(matches);
    }

    chfl_free(frame);
    chfl_free(selection);
    chfl_trajectory_close(input);
    chfl_trajectory_close(output);

    return 0;
}

int compare_matches(const void* lhs, const void* rhs) {
    size_t left = *(const size_t*)lhs;
    size_t right = *(const size_t*)rhs;

    if (left > right) return -1;
    if (right < left) return 1;
    return 0;
}
