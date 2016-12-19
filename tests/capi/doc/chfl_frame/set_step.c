#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_FRAME* frame = chfl_frame(42);

    chfl_frame_set_step(frame, 678);

    uint64_t step = 0;
    chfl_frame_step(frame, &step);
    assert(step == 678);

    chfl_frame_free(frame);
    // [example]
    return 0;
}
