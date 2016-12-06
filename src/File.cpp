// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/File.hpp"
#include "chemfiles/files/BasicFile.hpp"
using namespace chemfiles;

std::unique_ptr<TextFile> TextFile::create(const std::string& path, File::Mode mode) {
    // This function is currently creating only BasicFile. The idea here is to
    // be able to use memory mapped files, files on the network or gziped files
    // by only modifying this function.
    return std::unique_ptr<TextFile>(new BasicFile(path, mode));
}
