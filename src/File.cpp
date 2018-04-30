// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <algorithm>

#include "chemfiles/files/BasicFile.hpp"
#include "chemfiles/ErrorFmt.hpp"
using namespace chemfiles;


std::unique_ptr<TextFile> TextFile::create(const std::string& path, File::Mode mode) {
    // This function is currently creating only BasicFile. The idea here is to
    // be able to use memory mapped files, files on the network or gziped files
    // by only modifying this function.
    return std::unique_ptr<TextFile>(new BasicFile(path, mode));
}

TextFile::TextFile(const std::string& path, File::Mode mode, std::streambuf* buffer):
    File(path, mode), std::iostream(buffer)
{
    std::iostream::clear();
    // Throw exceptions on errors
    std::iostream::exceptions(std::fstream::badbit | std::fstream::failbit);
}

/// Read a single line from `stream` in `string`, handling all possible end of
/// line markers (`\n`, `\r`, `\r\n`).
///
/// Code from http://stackoverflow.com/a/6089413
void TextFile::get_line(std::string& string) {
    string.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.
    std::istream::sentry sentry(*this, true);
    std::streambuf* buffer = std::istream::rdbuf();

    while(true) {
        int c = buffer->sbumpc();
        switch (c) {
        case '\n':
            return;
        case '\r':
            if(buffer->sgetc() == '\n') {
                buffer->sbumpc();
            }
            return;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(string.empty()) {
                std::istream::setstate(std::ios::eofbit);
            }
            return;
        default:
            string += static_cast<char>(c);
        }
    }
}

std::string TextFile::readline() {
    std::string line;
    try {
        get_line(line);
    } catch (const std::ios_base::failure& e) {
        throw file_error("could not read a line in {}: {}", filename(), e.what());
    }
#if defined(__GNUC__) && (__GNUC__ == 5 || __GNUC__ == 6)
        // GCC 5 and 6 throw the wrong exception type for std::ios_base::failure
        // They still throw this exception using the C++03 ABI, while the above
        // is catching it using the C++11 ABI. So we manually check the exception
        // type here.
        //
        // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66145.
        catch(const std::exception& e) {
            if (std::string(typeid(e).name()) == "NSt8ios_base7failureE") {
                throw file_error("could not read a line in {}: {}", filename(), e.what());
            } else {
                throw e;
            }
        }
#endif
    return line;
}

void TextFile::rewind() {
    std::istream::clear();
    std::istream::seekg(0, std::ios::beg);
}

bool TextFile::eof() {
    return std::istream::eof();
}

std::vector<std::string> TextFile::readlines(size_t n) {
    auto lines = std::vector<std::string>(n);
    for (size_t i = 0; i < n; i++) {
        try {
            get_line(lines[i]);
        } catch (const std::ios_base::failure& e) {
            throw file_error("could not read a line in {}: {}", filename(), e.what());
        }
#if defined(__GNUC__) && (__GNUC__ == 5 || __GNUC__ == 6)
        // GCC 5 and 6 throw the wrong exception type for std::ios_base::failure
        // They still throw this exception using the C++03 ABI, while the above
        // is catching it using the C++11 ABI. So we manually check the exception
        // type here.
        //
        // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66145.
        catch(const std::exception& e) {
            if (std::string(typeid(e).name()) == "NSt8ios_base7failureE") {
                throw file_error("could not read a line in {}: {}", filename(), e.what());
            } else {
                throw e;
            }
        }
#endif
    }
    return lines;
}
