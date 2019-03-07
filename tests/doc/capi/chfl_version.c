// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <string.h>
#include <assert.h>

int main() {
    // [example]
    const char* version = chfl_version();
    assert(strcmp(version, CHEMFILES_VERSION) == 0);
    // [example]
    return 0;
}
