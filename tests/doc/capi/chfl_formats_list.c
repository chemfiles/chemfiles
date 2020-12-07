// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    // [example]
    chfl_format_metadata* formats = NULL;
    uint64_t count = 0;
    chfl_formats_list(&formats, &count);

    for (uint64_t i=0; i<count; i++) {
        printf("%s (%s)\n", formats[i].name, formats[i].extension == NULL ? "" : formats[i].extension);
    }

    // the user is responsible with freeing the memory
    chfl_free(formats);
    // [example]
    return 0;
}
