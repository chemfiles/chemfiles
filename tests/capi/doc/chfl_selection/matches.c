// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame();

    CHFL_ATOM* O = chfl_atom("O");
    CHFL_ATOM* H = chfl_atom("H");
    chfl_frame_add_atom(frame, O, (chfl_vector3d){0, 0, 0}, NULL);
    chfl_frame_add_atom(frame, H, (chfl_vector3d){1, 0, 0}, NULL);
    chfl_frame_add_atom(frame, H, (chfl_vector3d){0, 1, 0}, NULL);
    chfl_atom_free(O);
    chfl_atom_free(H);

    CHFL_SELECTION* selection = chfl_selection("name H");

    uint64_t size = 0;
    chfl_selection_evaluate(selection, frame, &size);
    assert(size == 2);

    chfl_match matches[2];
    chfl_selection_matches(selection, matches, 2);
    assert(matches[0].atoms[0] == 1);
    assert(matches[1].atoms[0] == 2);

    chfl_selection_free(selection);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
