#include "chemfiles.h"

// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    assert(strcmp(chfl_strerror(0), "Operation was sucessfull") == 0);
    assert(strcmp(chfl_last_error(), "") == 0);

    chfl_log_level_t level;
    assert(!chfl_loglevel(&level));
    assert(level == CHFL_LOG_WARNING);

    assert(!chfl_set_loglevel(CHFL_LOG_NONE));
    assert(!chfl_loglevel(&level));
    assert(level == CHFL_LOG_NONE);

    assert(!chfl_logfile("test.log"));

    // Check for file existence
    FILE* file = fopen("test.log","r");
    assert(file != NULL);
    fclose(file);

    assert(!chfl_log_stderr());
    remove("test.log");
    return EXIT_SUCCESS;
}
