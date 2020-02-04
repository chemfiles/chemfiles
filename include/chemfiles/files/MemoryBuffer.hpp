// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_MEMORY_BUFFER_HPP
#define CHEMFILES_MEMORY_BUFFER_HPP

#include <vector>
#include "chemfiles/File.hpp"
#include "chemfiles/external/span.hpp"

namespace chemfiles {

/// A class for handling memory passed directly instead of through a file
/// handle. Unlike a `std::vector`, it does not assume ownership of the data
/// when initialized through a pointer and size.
class MemoryBuffer final {
public:

    /// Create a MemoryBuffer intended for writting.
    MemoryBuffer(size_t initial_size = 20480)
        : write_mem_(), begin_(nullptr), size_(0)
    {
        write_mem_.reserve(initial_size);
        begin_ = write_mem_.data();
    }

    /// Create a MemoryBuffer intended for reading memory external to the class.
    MemoryBuffer(const char* memory, size_t size)
        : begin_(memory), size_(size) {}


    span<char> write_memory_as_string() {
        return span<char>(write_mem_.data(), write_mem_.size());
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
