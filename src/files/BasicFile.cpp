/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp/files/BasicFile.hpp"
#include <algorithm>

using namespace harp;

BasicFile::BasicFile(const std::string& _filename, const std::string& _mode)
: TextFile(_filename) {
    std::ios_base::openmode mode;
    if (_mode == "r") {
        mode = std::ios_base::in;
    } else if (_mode == "w") {
        mode = std::ios_base::out | std::ios_base::app;
    } else {
        throw FileError("Unrecognized file mode: " + _mode);
    }

    stream.open(_filename, mode);
    if (!stream.is_open())
        throw FileError("Could not open the file " + _filename);
    lines.resize(1);
    rewind();
}

BasicFile::~BasicFile(){
    stream.close();
}


const std::string& BasicFile::getline(){
    *this >> lines[0];
    return lines[0];
}

BasicFile& BasicFile::operator>>(std::string& line){
    std::getline(stream, line);
    return *this;
}

const std::vector<std::string>& BasicFile::readlines(size_t n){
    lines.resize(n);
    std::string line;
    for (size_t i=0; i<n; i++){
        std::getline(stream, line);
        lines[i] = line;
    }

    if (!stream)
        throw FileError("Error while reading file " + filename());

    return lines;
}

size_t BasicFile::nlines(){
    auto position = stream.tellg();
    rewind();
    size_t n = static_cast<size_t>(
                std::count(std::istreambuf_iterator<char>(stream),
                           std::istreambuf_iterator<char>(),
                           '\n'));
    n += 1; // The 1 is here because of the 0-based indexing in C++
    stream.seekg(position);
    return n;
}

void BasicFile::writeline(const std::string& line){
    *this << line;
}

BasicFile& BasicFile::operator<<(const any& data){
    stream << data;
    return *this;
}

void BasicFile::writelines(const std::vector<std::string>& _lines){
    for (auto line : _lines)
        *this << line;
}
