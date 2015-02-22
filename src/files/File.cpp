/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "files/File.hpp"
#include <algorithm>

using namespace harp;

BasicFile::BasicFile(const std::string& _filename) : TextFile(_filename),
stream(_filename, std::ios_base::in | std::ios_base::out | std::ios_base::app) {
    if (!stream.is_open()) {
        throw FileError("Could not open the file " + filename);
    }
    lines.reserve(1);
    rewind();
}

BasicFile::~BasicFile(){
    stream.close();
}


const std::string& BasicFile::getline(void){
    *this >> lines[0];
    return lines[0];
}

BasicFile& BasicFile::operator>>(std::string& line){
    std::getline(stream, line);
    return *this;
}

const std::vector<std::string>& BasicFile::readlines(size_t n){
    lines.reserve(n);
    size_t initial_size = lines.size();
    std::string line;
    for (size_t i=0; i<n; i++){
        std::getline(stream, line);
        if (i < initial_size)
            lines[i] = line;
        else
            lines.push_back(line);
    }

    if (not stream)
        throw FileError("Error while reading file " + filename);

    if (lines.size() > 2*n)
        lines.shrink_to_fit();

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

BasicFile& BasicFile::operator<<(const std::string& line){
    stream << line << std::endl;
    return *this;
}

void BasicFile::writelines(const std::vector<std::string>& lines){
    for (auto line : lines)
        *this << line;
}
