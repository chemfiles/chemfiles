// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Error.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/files/MMTFFile.hpp"

using namespace chemfiles;

MMTFFile::MMTFFile(std::string filename, File::Mode mode):
  File(std::move(filename), mode), handle_(MMTF_container_new()) {

    if (handle_ == nullptr) {
        throw memory_error("Could not allocate a MMTF container");
    }

    if (mode == File::READ) {
        bool result = MMTF_unpack_from_file(this->filename().c_str(), handle_);
        if (!result) {
            throw format_error("Issue with: {}. Please ensure it is valid MMTF", this->filename());
        }
    }

    if (mode == File::WRITE || mode == File::APPEND) {
        throw file_error("Writting an MMTF File is not currently implemented");
    }
}

MMTFFile::~MMTFFile() noexcept {
    MMTF_container_free(handle_);
}
