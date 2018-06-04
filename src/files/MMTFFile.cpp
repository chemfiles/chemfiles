// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Error.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/files/MMTFFile.hpp"

#include <mmtf.hpp>

using namespace chemfiles;

MMTFFile::MMTFFile(std::string filename, File::Mode mode):
  File(std::move(filename), mode), handle_() {

    if (mode == File::READ) {
        mmtf::decodeFromFile(handle_, this->filename());
        if (!handle_.hasConsistentData()) {
            throw format_error("Issue with: {}. Please ensure it is valid MMTF", this->filename());
        }
    }

    if (mode == File::WRITE || mode == File::APPEND) {
        throw file_error("Writting an MMTF File is not currently implemented");
    }
}

MMTFFile::~MMTFFile() noexcept {
}
