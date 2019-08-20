// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstdio>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "chemfiles/File.hpp"
#include "chemfiles/files/GzFile.hpp"
#include "chemfiles/files/PlainFile.hpp"
#include "chemfiles/files/XzFile.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/unreachable.hpp"

using namespace chemfiles;

std::unique_ptr<TextFile> TextFile::open(std::string path, File::Mode mode, File::Compression compression) {
    switch (compression) {
    case File::DEFAULT:
        return std::unique_ptr<TextFile>(new PlainFile(std::move(path), mode));
    case File::GZIP:
        return std::unique_ptr<TextFile>(new GzFile(std::move(path), mode));
    case File::LZMA:
        return std::unique_ptr<TextFile>(new XzFile(std::move(path), mode));
    }
    unreachable();
}

TextFile::TextFile(std::string path, File::Mode mode, File::Compression compression, std::streambuf* buffer):
    File(std::move(path), mode, compression), stream_(buffer)
{
    stream_.clear();
    // Throw exceptions on errors
    stream_.exceptions(std::fstream::badbit | std::fstream::failbit);
}

/// Read a single line from `stream` in `string`, handling all possible end of
/// line markers (`\n`, `\r`, `\r\n`).
///
/// Code from http://stackoverflow.com/a/6089413
void TextFile::get_line_impl(std::string& string) {
    string.clear();
    string.reserve(128);

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.
    std::istream::sentry sentry(stream_, true);
    std::streambuf* buffer = stream_.rdbuf();

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
                stream_.setstate(std::ios::eofbit);
            }
            return;
        default:
            string.push_back(static_cast<char>(c));
        }
    }
}

void TextFile::skip_line_impl() {
    std::istream::sentry sentry(stream_, true);
    std::streambuf* buffer = stream_.rdbuf();

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
            stream_.setstate(std::ios::eofbit);
            return;
        default:
            continue;
        }
    }
}

void TextFile::rewind() {
    this->clear();
    this->seekpos(0);
}

std::string TextFile::readline() {
    // Disable exceptions checking, and manually check bellow
    auto state = stream_.exceptions();
    stream_.exceptions(std::fstream::goodbit);

    std::string line;
    get_line_impl(line);
    if (stream_.fail()) {
        throw file_error("could not read a line in {}", this->path());
    }

    // Re-enable exceptions checking
    stream_.exceptions(state);
    return line;
}

std::vector<std::string> TextFile::readlines(size_t n) {
    // Disable exceptions checking, and manually check bellow
    auto state = stream_.exceptions();
    stream_.exceptions(std::fstream::goodbit);

    auto lines = std::vector<std::string>(n);
    for (size_t i = 0; i < n; i++) {
        get_line_impl(lines[i]);
    }

    if (stream_.fail()) {
        throw file_error("could not read a line in {}", this->path());
    }

    // Re-enable exceptions checking
    stream_.exceptions(state);

    return lines;
}

void TextFile::skipline() {
    // Disable exceptions checking, and manually check bellow
    auto state = stream_.exceptions();
    stream_.exceptions(std::fstream::goodbit);

    skip_line_impl();
    if (stream_.fail()) {
        throw file_error("could not read a line in {}", this->path());
    }

    // Re-enable exceptions checking
    stream_.exceptions(state);
}

void TextFile::skiplines(size_t n) {
    // Disable exceptions checking, and manually check bellow
    auto state = stream_.exceptions();
    stream_.exceptions(std::fstream::goodbit);

    for (size_t i = 0; i < n; i++) {
        skip_line_impl();
    }

    if (stream_.fail()) {
        throw file_error("could not read a line in {}", this->path());
    }

    // Re-enable exceptions checking
    stream_.exceptions(state);
}

std::streampos TextFile::tellpos() {
    return stream_.tellg();
}

void TextFile::seekpos(std::streampos position) {
    stream_.seekg(position);
}

bool TextFile::good() const {
    return stream_.good();
}

bool TextFile::eof() const {
    return stream_.eof();
}

bool TextFile::fail() const {
    return stream_.fail();
}

bool TextFile::bad() const {
    return stream_.bad();
}

void TextFile::clear() {
    stream_.clear();
}
