#include <chemfiles.h>
#include <stdlib.h>

int main() {
    // [example] [no-run]
    CHFL_TRAJECTORY* trajectory = chfl_trajectory_open("water.xyz", 'r');

    if (trajectory == NULL) {
        /* handle error */
    }

    chfl_trajectory_close(trajectory);
    // [example]
    return 0;
}
