// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/files/MemoryFile.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/unreachable.hpp"

using namespace chemfiles;

void MemoryFile::clear() noexcept {
}

void MemoryFile::seek(uint64_t position) {

    if (mode_ != File::READ) {
        throw file_error("cannot seek a memory file unless it is opened in read mode");
    }

    if (position > data_.size()) {
        current_location_ =  data_.size();
    }

    current_location_ = position;
}

size_t MemoryFile::read(char* data, size_t count) {

    if (mode_ != File::READ) {
        throw file_error("cannot read a memory file unless it is opened in read mode");
    }

    if (current_location_ >= data_.size()) {
        return 0;
    }

    auto amount_to_read = count + current_location_ <= data_.size() ?
        count : data_.size() - current_location_;

    std::copy(data_.data() + current_location_,
              data_.data() + current_location_ + amount_to_read,
              data);

    current_location_ += amount_to_read;

    return amount_to_read;
}

void MemoryFile::write(const char* data, size_t count) {
    if (mode_ != File::WRITE) {
        throw file_error("cannot write to a memory file unless it is opened in write mode");
    }

    std::copy(data, data + count, std::back_inserter(data_.write_memory()));
}
