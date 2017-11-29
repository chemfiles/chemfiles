// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/File.hpp"
#include "chemfiles/files/BasicFile.hpp"
using namespace chemfiles;

std::unique_ptr<TextFile> TextFile::create(const std::string& path, File::Mode mode) {
    // This function is currently creating only BasicFile. The idea here is to
    // be able to use memory mapped files, files on the network or gziped files
    // by only modifying this function.
    return std::unique_ptr<TextFile>(new BasicFile(path, mode));
}
