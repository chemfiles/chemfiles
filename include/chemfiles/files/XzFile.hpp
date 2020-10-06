// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_XZ_FILES_HPP
#define CHEMFILES_XZ_FILES_HPP

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include <lzma.h>

#include "chemfiles/File.hpp"
#include "chemfiles/files/MemoryBuffer.hpp"

namespace chemfiles {

/// An implementation of TextFile for lzma/xz files
class XzFile final: public TextFileImpl {
public:
    /// Open a text file with name `filename` and mode `mode`.
    XzFile(const std::string& path, File::Mode mode);
    ~XzFile() override;

    size_t read(char* data, size_t count) override;
    void write(const char* data, size_t count) override;

    void clear() noexcept override;
    void seek(uint64_t position) override;

private:
    /// Compress data from stream_.next_in, and write the data to the file.
    /// If action==LZMA_FINISH, continue writing until everything has been
    /// processed.
    void compress_and_write(lzma_action action);

    FILE* file_ = nullptr;
    /// Store opening file mode
    File::Mode mode_;
    /// lzma stream used both for reading and writing. Reading is done using
    /// lzma_stream_decoder, and writing using lzma_easy_encoder.
    lzma_stream stream_ = LZMA_STREAM_INIT;
    /// compressed data buffer, straight out from the file when reading, to be
    /// written to the file when writing.
    std::vector<uint8_t> buffer_;
};

/// Inflates LZMA/XZ data from the `src` buffer
MemoryBuffer decompress_xz(const char* src, size_t size);

}

#endif
