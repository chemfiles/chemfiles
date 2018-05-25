// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_XZ_FILES_HPP
#define CHEMFILES_XZ_FILES_HPP

#include <streambuf>
#include <vector>
#include <array>
#include <cstdio>

#include <lzma.h>

#include "chemfiles/File.hpp"

namespace chemfiles {

class xzstreambuf : public std::streambuf {
public:
    xzstreambuf(const std::string& path, const std::string& mode, size_t buffer_size = 1024);
    virtual ~xzstreambuf() override;

    bool is_open() const;

protected:
    int underflow() override;
    int overflow(int c) override;
    int sync() override;
    pos_type seekoff(off_type offset, std::ios_base::seekdir way, std::ios_base::openmode which) override;
    pos_type seekpos(pos_type position, std::ios_base::openmode which) override;

private:
    void replenish_compressed_buffer();
    bool init_index();

private:
    FILE* file_;

    /// lzma stream used both for reading and writing. Reading is done through
    /// a block stream, and writing using a ???
    lzma_stream stream_;
    /// Current block (used when reading). This must be a class member, as a
    /// pointer to this will be saved in stream_
    lzma_block block_;
    /// Buffer for storing lzma_filter in the block_
    std::array<lzma_filter, LZMA_FILTERS_MAX + 1> filters_;
    /// Action for sync. Only used when writing
    lzma_action action_;
    /// Check used by the stream for decoding
    lzma_check check_;
    /// Optional index for seekoff/seekpos
    lzma_index* index_;

    std::vector<char> in_buffer_;
    std::vector<char> out_buffer_;

    uint64_t decoded_position_;
    uint64_t discard_amount_;
    bool at_block_boundary_;
    bool reading_;
};

/// A xz-compressed text file
class XzFile final: public TextFile {
public:
    /// Open the file with the given `filename` using the specified `mode`
    XzFile(const std::string& filename, File::Mode mode);

private:
    xzstreambuf buffer_;
};

}

#endif
