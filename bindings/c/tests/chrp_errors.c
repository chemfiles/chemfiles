#include "chemharp.h"

// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    assert(strcmp(chrp_strerror(0), "Operation was sucessfull") == 0);
    assert(strcmp(chrp_last_error(), "") == 0);

    chrp_log_level_t level;
    assert(!chrp_loglevel(&level));
    assert(level == CHRP_LOG_WARNING);

    assert(!chrp_set_loglevel(CHRP_LOG_NONE));
    assert(!chrp_loglevel(&level));
    assert(level == CHRP_LOG_NONE);

    assert(!chrp_logfile("test.log"));

    // Check for file existence
    FILE* file = fopen("test.log","r");
    assert(file != NULL);
    fclose(file);

    assert(!chrp_log_stderr());
    remove("test.log");
    return EXIT_SUCCESS;
}
