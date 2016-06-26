/* File select.c, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <stdio.h>
#include <stdlib.h>

#include "chemfiles.h"

int main() {
    CHFL_TRAJECTORY* file = chfl_trajectory_open("filename.xyz", 'r');
    CHFL_FRAME* frame = chfl_frame(0);
    if(!chfl_trajectory_read(file, frame)) {/*Handle error*/}

    // Create a selection for all atoms with "Zn" name
    CHFL_SELECTION* selection = chfl_selection("name Zn");
    size_t matching = 0;

    // Get the number of matching atoms from the frame
    chfl_selection_evalutate(selection, frame, &matching);
    printf("We have %zu zinc in the frame\n", matching);
    chfl_match_t* matches = malloc(matching * sizeof(chfl_match_t));

    // Get the matching atoms
    chfl_selection_matches(selection, matches, matching);
    for (size_t i=0; i<matching; i++) {
        printf("%zu is a zinc\n", matches[i].atoms[0]);
    }

    chfl_selection_free(selection);
    free(matches);

    // Create a selection for multiple atoms
    selection = chfl_selection("angles: name($1) H and name($2) O and name($3) H");
    chfl_selection_evalutate(selection, frame, &matching);
    printf("We have %zu water in the frame\n", matching);
    matches = malloc(matching * sizeof(chfl_match_t));

    // Get the matching atoms
    chfl_selection_matches(selection, matches, matching);
    for (size_t i=0; i<matching; i++) {
        printf(
            "%zu - %zu - %zu is a water\n",
            matches[i].atoms[0], matches[i].atoms[1], matches[i].atoms[2]
        );
    }

    return 0;
}
