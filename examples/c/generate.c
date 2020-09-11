/* This file is an example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <chemfiles.h>
#include <stdlib.h>

int main(void) {
    CHFL_FRAME* frame = chfl_frame();
    CHFL_ATOM* H = chfl_atom("H");
    CHFL_ATOM* O = chfl_atom("O");

    chfl_frame_add_atom(frame, H, (chfl_vector3d){1, 0, 0}, NULL);
    chfl_frame_add_atom(frame, O, (chfl_vector3d){0, 0, 0}, NULL);
    chfl_frame_add_atom(frame, H, (chfl_vector3d){0, 1, 0}, NULL);

    chfl_frame_add_bond(frame, 0, 1);
    chfl_frame_add_bond(frame, 2, 1);

    CHFL_CELL* cell = chfl_cell((chfl_vector3d){10, 10, 10}, NULL);
    chfl_frame_set_cell(frame, cell);
    chfl_free(cell);

    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.pdb", 'w');
    chfl_trajectory_write(trajectory, frame);
    chfl_trajectory_close(trajectory);

    chfl_free(frame);
    chfl_free(H);
    chfl_free(O);

    return 0;
}
