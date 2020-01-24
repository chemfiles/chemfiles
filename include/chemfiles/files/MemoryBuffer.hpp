// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_MEMORY_BUFFER_HPP
#define CHEMFILES_MEMORY_BUFFER_HPP

#include <vector>
#include "chemfiles/File.hpp"

namespace chemfiles {

/// A class for handling memory passed directly instead of through a file
/// handle. Unlike a `std::vector`, it does not assume ownership of the data
/// when initialized through a pointer and size.
class MemoryBuffer final {
public:

    /// Create a MemoryBuffer intended for writting.
    MemoryBuffer(size_t initial_size = 20480)
        : write_mem_(initial_size, '\0'), begin_(write_mem_.data()), size_(write_mem_.size())
    {}

    /// Create a MemoryBuffer intended for reading memory external to the class.
    MemoryBuffer(const char* memory, size_t size)
        : begin_(memory), size_(size) {}

    /// Create a MemoryBuffer intended for reading memory owned by the class.
    MemoryBuffer(std::vector<char> memory)
        : write_mem_(std::move(memory)), begin_(write_mem_.data()), size_(write_mem_.size())
    {}

    void reset(std::vector<char> memory) {
        write_mem_ = std::move(memory);
        begin_ = write_mem_.data();
        size_ = write_mem_.size();
    }

    void grow() {
        write_mem_.resize(write_mem_.size() * 2, '\0');
        begin_ = write_mem_.data();
        size_ = write_mem_.size();
    }

    std::vector<char>& write_memory() {
        return write_mem_;
    }

    size_t size() const {
        return size_;
    }

    const char* data() const {
        return begin_;
    }

    /// Try to decompress the content of this buffer with the given
    /// `compression` format
    void decompress(File::Compression compression);

private:

    /// Memory controlled by this class, typically used for writting
    std::vector<char> write_mem_;

    /// Read-only memory
    const char* begin_;

    /// Size of read-only memory
    size_t size_;
};

}

#endif
