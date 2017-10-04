// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <string.h>
#include <assert.h>

// -- silent clang warnings
extern char LAST_WARNING[1024];
void warning_callback(const char* message);
// -- end of clang warnings

    // [example]
    char LAST_WARNING[1024] = {0};

    void warning_callback(const char* message) {
        strncpy(LAST_WARNING, message, 1024);
        LAST_WARNING[1023] = '\0';
    }

    int main() {
        chfl_set_warning_callback(warning_callback);

        // Generate a warning event
        chfl_trajectory_open("noformat", 'r');

        assert(strcmp(LAST_WARNING, "file at 'noformat' does not have an extension, provide a format name to read it") == 0);
        return 0;
    }

    // [example]
