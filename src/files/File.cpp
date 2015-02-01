/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "files/File.hpp"

using namespace harp;

TextFile::TextFile(std::string filename) :
stream(filename, std::ios_base::in | std::ios_base::out | std::ios_base::app) {
    if (!stream.is_open()) {
        throw HarpFileError("Could not open the file " + filename);
    }
    lines.reserve(1);
    rewind();
}

TextFile::~TextFile(){
    stream.close();
}


const std::string& TextFile::getline(void){
    *this >> lines[0];
    return lines[0];
}

TextFile& TextFile::operator>>(std::string& line){
    std::getline(stream, line);
    return *this;
}

const std::vector<std::string>& TextFile::readlines(int n){
    lines.reserve(n);
    int initial_size = lines.size();
    std::string line;
    for (int i=0; i<n; i++){
        std::getline(stream, line);
        if (i < initial_size)
            lines[i] = line;
        else
            lines.push_back(line);
    }

    if (lines.size() > 2*n)
        lines.shrink_to_fit();

    return lines;
}

int TextFile::nlines(){
    auto position = stream.tellg();
    rewind();
    int n = std::count(std::istreambuf_iterator<char>(stream),
                       std::istreambuf_iterator<char>(),
                       '\n') + 1; // The 1 is here because of the 0-based
                                  // indexing in C++
    stream.seekg(position);
    return n;
}

void TextFile::writeline(const std::string& line){
    *this << line;
}

TextFile& TextFile::operator<<(const std::string& line){
    stream << line << std::endl;
    return *this;
}

void TextFile::writelines(const std::vector<std::string>& lines){
    for (auto line : lines)
        *this << line;
}
