#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame(42);
    CHFL_FRAME* copy = chfl_frame_copy(frame);

    if (copy == NULL) {
        /* handle error */
    }

    chfl_frame_free(copy);
    chfl_frame_free(frame);
    // [example]
    return 0;
}
