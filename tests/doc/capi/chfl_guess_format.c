// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <string.h>
#include <assert.h>

int main() {
    // [example]
    char format[256] = {0};
    chfl_guess_format("filename.nc", format, sizeof(format));
    assert(strcmp(format, "Amber NetCDF") == 0);

    chfl_guess_format("filename.xyz.gz", format, sizeof(format));
    assert(strcmp(format, "XYZ / GZ") == 0);
    // [example]
    return 0;
}
