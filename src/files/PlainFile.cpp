// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <algorithm>

#include "chemfiles/files/PlainFile.hpp"
#include "chemfiles/ErrorFmt.hpp"
using namespace chemfiles;

PlainFile::PlainFile(std::string path, File::Mode mode)
    : TextFile(std::move(path), mode, File::DEFAULT, &buffer_), buffer_() {
    // We need to use binary mode when opening the file because we are storing
    // positions in the files relative to line ending positions. Using text
    // mode make the MSVC runtime convert lines ending and then all the values
    // return by tellg are wrong.
    //
    // We can do this because we are dealing with line ending ourself in the
    // `get_line` function.
    std::ios_base::openmode openmode = std::ios_base::binary;

    switch (mode) {
    case File::READ:
        openmode |= std::ios_base::in;
        break;
    case File::APPEND:
        openmode |= std::ios_base::out | std::ios_base::app;
        break;
    case File::WRITE:
        openmode |= std::ios_base::out | std::ios_base::trunc;
        break;
    }

    buffer_.open(this->path(), openmode);
    if (!buffer_.is_open()) {
        throw file_error("could not open the file at {}", this->path());
    }
}
