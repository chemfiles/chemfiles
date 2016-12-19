#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame(42);

    chfl_frame_add_velocities(frame);

    bool velocities = false;
    chfl_frame_has_velocities(frame, &velocities);
    assert(velocities = true);

    chfl_frame_free(frame);
    // [example]
    return 0;
}
