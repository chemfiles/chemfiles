// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chemfiles.h"
#include "helpers.h"

// Global variables for access from callback and main
char* buffer;
chfl_log_level_t last_level;

void callback(chfl_log_level_t level, const char* message) {
    size_t size = strlen(message) + 1;
    buffer = malloc(sizeof(char)*size);
    strcpy(buffer, message);
    last_level = level;
}

int main() {
    silent_crash_handlers();
    assert(strcmp(chfl_strerror(CHFL_SUCCESS), "operation was sucessfull") == 0);
    assert(strcmp(chfl_last_error(), "") == 0);

    chfl_log_level_t level;
    assert(!chfl_loglevel(&level));
    assert(level == CHFL_LOG_WARNING);

    assert(!chfl_set_loglevel(CHFL_LOG_DEBUG));
    assert(!chfl_loglevel(&level));
    assert(level == CHFL_LOG_DEBUG);

    assert(!chfl_logfile("test.log"));

    // Check for file existence (it must be opened in binary mode for the
    // good behaviour of read_whole_file on Windows)
    FILE* file = fopen("test.log","rb");
    assert(file != NULL);

    assert(chfl_trajectory_open("noformat", 'r') == NULL);
    assert(!chfl_log_stderr());

    char* content = read_whole_file("test.log");
    assert(strcmp(content, "Chemfiles error: Can not find a format associated with the \"\" extension.\n") == 0);

    free(content);
    fclose(file);

    // Test callback-based logging
    assert(!chfl_log_callback(callback));
    assert(chfl_trajectory_open("noformat", 'r') == NULL);
    assert(strcmp(buffer, "Can not find a format associated with the \"\" extension.") == 0);
    assert(last_level == CHFL_LOG_ERROR);
    free(buffer);

    remove("test.log");

    assert(!chfl_log_stdout());

    assert(strcmp(chfl_last_error(), "") != 0);
    assert(!chfl_clear_errors());
    assert(strcmp(chfl_last_error(), "") == 0);

    char* version = read_whole_file(SRCDIR "/VERSION");
    assert(version != NULL);
    // Remove the trailing \n
    version[strlen(version) - 1] = 0;

    assert(strstr(chfl_version(), version) != NULL);

    free(version);

    return EXIT_SUCCESS;
}
