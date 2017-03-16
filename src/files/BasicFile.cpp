// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <algorithm>

#include "chemfiles/files/BasicFile.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

/// Read a single line from `stream` in `string`, handling all possible end of
/// line markers (`\n`, `\r`, `\r\n`).
///
/// Code from http://stackoverflow.com/a/6089413
static inline std::istream& get_line(std::istream& istream, std::string& string) {
    string.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.
    std::istream::sentry sentry(istream, true);
    std::streambuf* buffer = istream.rdbuf();

    while(true) {
        int c = buffer->sbumpc();
        switch (c) {
        case '\n':
            return istream;
        case '\r':
            if(buffer->sgetc() == '\n')
                buffer->sbumpc();
            return istream;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(string.empty()) {
                istream.setstate(std::ios::eofbit);
            }
            return istream;
        default:
            string += static_cast<char>(c);
        }
    }
}

BasicFile::BasicFile(const std::string& filename, File::Mode mode)
    : TextFile(filename, mode) {
    std::ios_base::openmode openmode;

    switch (mode) {
    case File::READ:
        openmode = std::ios_base::in;
        break;
    case File::APPEND:
        openmode = std::ios_base::out | std::ios_base::app;
        break;
    case File::WRITE:
        openmode = std::ios_base::out | std::ios_base::trunc;
        break;
    default:
        throw FileError(std::string("Got a bad file mode: ") + static_cast<char>(mode));
    }

    stream_.open(filename, openmode);
    if (!stream_.is_open()) {
        throw FileError("Could not open the file " + filename);
    }
    TextFile::rdbuf(stream_.rdbuf());
    rewind();
    stream_.clear();
    // Throw exceptions on errors
    stream_.exceptions(std::fstream::badbit | std::fstream::failbit);
}

std::string BasicFile::readline() {
    std::string line;
    try {
        get_line(stream_, line);
    } catch (const std::ios_base::failure&) {
        throw FileError("Could not read a line in " + filename());
    }
    return line;
}

void BasicFile::rewind() {
    stream_.clear();
    stream_.seekg(0, std::ios::beg);
}

bool BasicFile::eof() {
    return stream_.eof();
}

std::vector<std::string> BasicFile::readlines(size_t n) {
    auto lines = std::vector<std::string>(n);
    for (size_t i = 0; i < n; i++) {
        try {
            get_line(stream_, lines[i]);
        } catch (const std::ios_base::failure&) {
            throw FileError("Could not read lines in " + filename());
        }
    }
    return lines;
}
