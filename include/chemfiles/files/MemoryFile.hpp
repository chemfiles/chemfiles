// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_MEMORY_FILES_HPP
#define CHEMFILES_MEMORY_FILES_HPP

#include "chemfiles/File.hpp"
#include "chemfiles/files/MemoryBuffer.hpp"

#include <istream> // Needed for clang
#include <ostream> // Needed for clang
#include <iosfwd>

namespace chemfiles {

/// Simple TextFile implementation, that wraps memory for reading only.
class MemoryFileReader final: public TextFileImpl {
    class vector_buffer_reader : public std::basic_streambuf<char, std::char_traits<char>> {
    public:
        vector_buffer_reader(const MemoryBuffer& memory) {
            auto non_const = const_cast<char*>(memory.data());
            setg(non_const, non_const, non_const + memory.size());
        }
    protected:
        std::streampos seekpos(std::streampos sp, std::ios_base::openmode mode = std::ios_base::in);
    };

public:
    /// Open `memory` as though it were a file in mode `mode`. No copy of `memory` is
    /// made and the original object **MUST** not be freed until this object is destroyed
    MemoryFileReader(const MemoryBuffer& memory)
        : TextFileImpl(""), buffer_(memory), data_(&buffer_)
    {}

    size_t read(char* data, size_t count) override;
    void write(const char* data, size_t count) override;

    void clear() noexcept override;
    void seek(uint64_t position) override;

private:

    /// The memory buffer
    vector_buffer_reader buffer_;

    /// An input-out stream that performs all the read operations
    std::istream data_;
};

/// Simple TextFile implementation, that wraps memory for writting only
class MemoryFileWriter final: public TextFileImpl {
    class vector_buffer_writer : public std::basic_streambuf<char, std::char_traits<char>> {
    public:
        vector_buffer_writer(MemoryBuffer& vec): original_vector_(vec) {
            auto& memory = original_vector_.write_memory();
            setp(memory.data(), memory.data() + memory.size());
        }
    protected:
        MemoryBuffer& original_vector_;

        using int_type = std::basic_streambuf<char>::int_type;
        int_type overflow(int_type ch);
        std::streampos seekpos(std::streampos sp, std::ios_base::openmode mode = std::ios_base::in);
    };

public:
    /// Open `memory` as though it were a file in mode `mode`. No copy of `memory` is
    /// made and the original object not be freed until this object is destroyed
    MemoryFileWriter(MemoryBuffer& memory)
        : TextFileImpl(""), buffer_(memory), data_(&buffer_)
    {}

    size_t read(char* data, size_t count) override;
    void write(const char* data, size_t count) override;

    void clear() noexcept override;
    void seek(uint64_t position) override;

private:

    /// The memory buffer
    vector_buffer_writer buffer_;

    /// An input-out stream that performs all the read/write operations
    std::ostream data_;
};

} // namespace chemfiles

#endif
