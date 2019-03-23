// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_GZ_FILES_HPP
#define CHEMFILES_GZ_FILES_HPP

#include <streambuf>
#include <vector>

#include <zlib.h>

#include "chemfiles/File.hpp"

namespace chemfiles {

/// An implementation of std::streambuf for gziped files
class gzstreambuf final: public std::streambuf {
public:
    gzstreambuf(size_t buffer_size = 512);
    ~gzstreambuf() override;

    gzstreambuf(const gzstreambuf&) = delete;
    gzstreambuf& operator=(const gzstreambuf&) = delete;
    gzstreambuf(gzstreambuf&&) noexcept = delete;
    gzstreambuf& operator=(gzstreambuf&&) noexcept = delete;

    /// Open the file at `path` with the given `mode`. The mode will be passed
    /// down to gzopen.
    void open(const std::string& path, const std::string& mode);

    int underflow() override;
    int overflow(int ch) override;

    int sync() override;
    bool is_open() const;

    std::streampos seekpos(std::streampos offset, std::ios_base::openmode mode) override {
        return pubseekoff(offset, std::ios_base::beg, mode);
    }

    std::streampos seekoff(std::streamoff offset, std::ios_base::seekdir way, std::ios_base::openmode mode) override;

private:
    std::vector<char> buffer_;
    gzFile file_ = nullptr;
};


/// A gziped text file
class GzFile final: public TextFile {
public:
    /// Open the file at the given `path` using the specified `mode`
    GzFile(std::string path, File::Mode mode);

private:
    gzstreambuf buffer_;
};

} // namespace chemfiles

#endif
