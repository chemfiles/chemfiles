/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <algorithm>

#include "chemfiles/files/BasicFile.hpp"
#include "chemfiles/Logger.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

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
        Logger::error(std::string("Got a bad file mode: ") + static_cast<char>(mode));
        abort();
        break;
    }

    stream_.open(filename, openmode);
    if (!stream_.is_open()) {
        throw FileError("Could not open the file " + filename);
    }
    TextFile::rdbuf(stream_.rdbuf());
    lines_.resize(1);
    rewind();
}

const std::string& BasicFile::getline() {
    *this >> lines_[0];
    return lines_[0];
}

void BasicFile::rewind() {
    stream_.clear();
    stream_.seekg(0, std::ios::beg);
}

bool BasicFile::eof() {
    return stream_.eof();
}

BasicFile& BasicFile::operator>>(std::string& line) {
    std::getline(stream_, line);
    return *this;
}

const std::vector<std::string>& BasicFile::readlines(size_t n) {
    lines_.resize(n);
    std::string line;
    for (size_t i = 0; i < n; i++) {
        std::getline(stream_, line);
        lines_[i] = line;
    }

    if (!stream_) {
        throw FileError("Error while reading file " + filename());
    }

    return lines_;
}

size_t BasicFile::nlines() {
    auto position = stream_.tellg();
    rewind();
    size_t n =
        static_cast<size_t>(std::count(std::istreambuf_iterator<char>(stream_),
                                       std::istreambuf_iterator<char>(), '\n'));
    n += 1; // The 1 is here because of the 0-based indexing in C++
    stream_.seekg(position);
    return n;
}

void BasicFile::writeline(const std::string& line) {
    *this << line;
}

void BasicFile::writelines(const std::vector<std::string>& _lines) {
    for (auto& line : _lines) {
        *this << line;
    }
}
