// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <string.h>
#include <assert.h>

int main() {
    // [example]
    chfl_add_configuration("local-file.toml");

    // reading a frame will now use atom names from the configuration
    // ...
    // [example]
    return 0;
}
