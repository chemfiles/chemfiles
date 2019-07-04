// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_XZ_FILES_HPP
#define CHEMFILES_XZ_FILES_HPP

#include <cstdio>
#include <cstdint>
#include <ios>
#include <array>
#include <vector>
#include <string>

#include <lzma.h>

#include "chemfiles/File.hpp"

namespace chemfiles {

/// An implementation of std::streambuf for lzma/xz files
class xzstreambuf final: public std::streambuf {
public:
    xzstreambuf(size_t buffer_size = 128 * 1024);
    ~xzstreambuf() override;

    xzstreambuf(const xzstreambuf&) = delete;
    xzstreambuf& operator=(const xzstreambuf&) = delete;
    xzstreambuf(xzstreambuf&&) = delete;
    xzstreambuf& operator=(xzstreambuf&&) = delete;

    /// Open the file at `path` with the given `mode`. The mode must be `rb` or
    /// `wb`.
    void open(const std::string& path, const std::string& mode);
    bool is_open() const;

protected:
    int underflow() override;
    int sync() override;
    pos_type seekoff(off_type offset, std::ios_base::seekdir way, std::ios_base::openmode which) override;
    pos_type seekpos(pos_type position, std::ios_base::openmode which) override;

private:
    void replenish_compressed_buffer();
    bool init_index();

private:
    FILE* file_ = nullptr;

    /// lzma stream used both for reading and writing. Reading is done through
    /// a block stream, and writing using lzma_easy_encoder.
    lzma_stream stream_ = LZMA_STREAM_INIT;
    /// Current block (used when reading). This must be a class member, as a
    /// pointer to this will be saved in stream_
    lzma_block block_ = lzma_block();
    /// Buffer for storing lzma_filter in the block_
    std::array<lzma_filter, LZMA_FILTERS_MAX + 1> filters_ = {{{LZMA_VLI_UNKNOWN, nullptr}}};
    /// Action for sync. Only used when writing
    lzma_action action_ = LZMA_RUN;
    /// Check used by the stream for decoding
    lzma_check check_;
    /// Optional index for seekoff/seekpos
    lzma_index* index_ = nullptr;

    std::vector<char> in_buffer_;
    std::vector<char> out_buffer_;

    uint64_t decoded_position_ = 0;
    uint64_t discard_amount_ = 0;
    bool at_block_boundary_ = true;
    bool reading_ = true;
};

/// A xz-compressed text file
class XzFile final: public TextFile {
public:
    /// Open the file at the given `path` using the specified `mode`
    XzFile(std::string path, File::Mode mode);

private:
    xzstreambuf buffer_;
};

}

#endif
