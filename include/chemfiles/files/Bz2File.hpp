// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_BZ2_FILES_HPP
#define CHEMFILES_BZ2_FILES_HPP

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>

#include "chemfiles/File.hpp"
#include "chemfiles/files/MemoryBuffer.hpp"

#include <bzlib.h>

namespace chemfiles {

/// An implementation of TextFile for bzip2 files
class Bz2File final: public TextFileImpl {
public:
    /// Open a text file with name `filename` and mode `mode`.
    Bz2File(const std::string& path, File::Mode mode);
    ~Bz2File() override;

    size_t read(char* data, size_t count) override;
    void write(const char* data, size_t count) override;

    void clear() noexcept override;
    void seek(uint64_t position) override;

private:
    void compress_and_write(int action);

    FILE* file_ = nullptr;
    /// Store the mode used to open this file
    File::Mode mode_;
    /// Store the right function to close the stream
    std::function<int(bz_stream*)> stream_end_;
    /// bzip2 stream used both for reading and writing.
    bz_stream stream_;
    /// compressed data buffer, straight out from the file when reading, to be
    /// written to the file when writing.
    std::vector<char> buffer_;
};

/// Inflates BZIP2 data from the `src` buffer
MemoryBuffer decompress_bz2(const char* src, size_t size);

}

#endif
