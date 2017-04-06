// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main() {
    // [example]
    CHFL_SELECTION* selection = chfl_selection("pairs: name(#1) O and name(#2) H");

    char string[64]= {0};
    chfl_selection_string(selection, string, sizeof(string));
    assert(strcmp(string, "pairs: name(#1) O and name(#2) H") == 0);

    chfl_selection_free(selection);
    // [example]
    return 0;
}
