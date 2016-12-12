#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame(10);
    /* Update the frame, or read it from a file */

    CHFL_ATOM* atom = chfl_atom_from_frame(frame, 4);

    if (atom == NULL) {
        /* handle error */
    }

    chfl_atom_free(atom);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
