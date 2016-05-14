#include "chemfiles.h"

// Force NDEBUG to be undefined
#undef NDEBUG
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if (defined(WIN32) || defined(WIN64))
#define EOL "\r\n"
#else
#define EOL "\n"
#endif

// Read a whole file at once
char* read_whole_file(FILE* file);
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
    assert(strcmp(chfl_strerror(CHFL_SUCCESS), "Operation was sucessfull") == 0);
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

    // Test writing to the file
    CHFL_TRAJECTORY* traj = chfl_trajectory_open("noformat", 'r');
    // Close the file and sync it with the HD
    assert(!chfl_log_stderr());

    char* content = read_whole_file(file);
    assert(strcmp(content, "Chemfiles error: Can not find a format associated with the \"\" extension." EOL) == 0);

    free(content);
    fclose(file);

    // Test callback-based logging
    assert(!chfl_log_callback(callback));
    traj = chfl_trajectory_open("noformat", 'r');
    assert(strcmp(buffer, "Can not find a format associated with the \"\" extension.") == 0);
    assert(last_level == CHFL_LOG_ERROR);
    free(buffer);

    remove("test.log");

    assert(strcmp(chfl_last_error(), "") != 0);
    assert(!chfl_clear_errors());
    assert(strcmp(chfl_last_error(), "") == 0);

    return EXIT_SUCCESS;
}

char* read_whole_file(FILE* file) {
    char *content = NULL;

    if (file != NULL){
        fseek(file, 0L, SEEK_END);
        size_t size = (size_t)ftell(file);
        rewind(file);
        content = (char*)malloc(sizeof(char)*(size + 1));

        if (content != NULL){
            fread(content, size, 1, file);
            content[size] = '\0';
        }
    }

    return content;
}
