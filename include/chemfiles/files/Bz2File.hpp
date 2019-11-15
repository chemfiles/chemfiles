// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_BZ2_FILES_HPP
#define CHEMFILES_BZ2_FILES_HPP

#include <bzlib.h>

#include "chemfiles/File.hpp"

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
    /// lzma stream used both for reading and writing. Reading is done using
    /// lzma_stream_decoder, and writing using lzma_easy_encoder.
    bz_stream stream_;
    /// compressed data buffer, straight out from the file when reading, to be
    /// written to the file when writing.
    std::vector<char> buffer_;
};

}

#endif
