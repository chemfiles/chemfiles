#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame(42);

    if (frame == NULL) {
        /* handle error */
    }

    chfl_frame_free(frame);
    // [example]
    return 0;
}
