// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <algorithm>

#include "chemfiles/files/BasicFile.hpp"
#include "chemfiles/ErrorFmt.hpp"
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
            if(buffer->sgetc() == '\n') {
                buffer->sbumpc();
            }
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

    stream_.open(filename, openmode);
    if (!stream_) {
        throw file_error("could not open the file at {}", filename);
    }
    TextFile::rdbuf(stream_.rdbuf());
    stream_.clear();
    // Throw exceptions on errors
    stream_.exceptions(std::fstream::badbit | std::fstream::failbit);
}

std::string BasicFile::readline() {
    std::string line;
    try {
        get_line(stream_, line);
    } catch (const std::ios_base::failure& e) {
        throw file_error("could not read a line in {}: {}", filename(), e.what());
    } catch(const std::exception& e) {
        // The libstdc++ have two ABI for std::ios_base::failure. A C++03
        // one, and a C++11 one. Depending on the version of the compiler
        // and the loaded libstdc++, the above catch might not work, so we
        // fallback here and manually test for the typeid
        auto id = std::string(typeid(e).name());
        if (id =="NSt8ios_base7failureE" || id == "NSt8ios_base7failureB5cxx11E") {
            throw file_error("could not read a line in {}: {}", filename(), e.what());
        } else {
            throw e;
        }
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
        } catch (const std::ios_base::failure& e) {
            throw file_error("could not read a line in {}: {}", filename(), e.what());
        } catch(const std::exception& e) {
            // The libstdc++ have two ABI for std::ios_base::failure. A C++03
            // one, and a C++11 one. Depending on the version of the compiler
            // and the loaded libstdc++, the above catch might not work, so we
            // fallback here and manually test for the typeid
            auto id = std::string(typeid(e).name());
            if (id =="NSt8ios_base7failureE" || id == "NSt8ios_base7failureB5cxx11E") {
                throw file_error("could not read a line in {}: {}", filename(), e.what());
            } else {
                throw e;
            }
        }
    }
    return lines;
}
