// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FILES_HPP
#define CHEMFILES_FILES_HPP

#include <istream>
#include <string>
#include <vector>
#include <memory>

#include <fmt/ostream.h>

#include "chemfiles/exports.h"

namespace chemfiles {

/// Abstract base class for file representation.
class CHFL_EXPORT File {
public:
    /// Possible modes for opening a file
    enum Mode: char {
        /// Open in read-only mode
        READ = 'r',
        /// Open in read-write mode, and replace the file if it is already present
        WRITE = 'w',
        /// Open in read-write mode, and append new data at the end of the file
        APPEND = 'a',
    };

    /// Possible compression methods for opening a file
    enum Compression {
        /// Default method: plain text or binary formats
        DEFAULT,
        /// gzip compression
        GZIP,
        /// lzma compression (.xz)
        LZMA,
    };

    virtual ~File() = default;
    File(File&&) = default;
    File& operator=(File&&) = default;
    File(File const&) = delete;
    File& operator=(File const&) = delete;

    /// Get the file path used to open this file.
    const std::string& path() const { return path_; }

    /// Get the mode used to open this file.
    Mode mode() const { return mode_; }

    /// Get the compression used to open this file.
    Compression compression() const { return compression_; }

protected:
    File(std::string path, Mode mode, Compression compression):
        path_(std::move(path)), mode_(mode), compression_(compression) {}

private:
    std::string path_;
    Mode mode_;
    Compression compression_;
};

/// Abstract base class representing a text file. This class is inteded to be
/// inherited by any form of text files: compressed files, memory-mapped files,
/// etc.
///
/// All failling operations should throw a `FileError` instead of waiting for
/// the user of the class to the current state.
class CHFL_EXPORT TextFile: public File {
public:
    /// Open the file at the given `path` with the requested `mode` and
    /// `compression` method using the most adapted child class of `TextFile`.
    static std::unique_ptr<TextFile> open(std::string path, File::Mode mode, File::Compression compression);

    /// Read a line from the file
    std::string readline();
    /// Read `n` lines from the file
    std::vector<std::string> readlines(size_t n);

    /// Skip a single line from the file
    void skipline();
    /// Skip `n` lines from the file
    void skiplines(size_t n);

    /// Reset the position indicator to the beggining of the file
    void rewind();

    /// returns the current position indicator
    std::streampos tellpos();
    /// set the position indicator to `position`
    void seekpos(std::streampos position);

    /// checks if no error has occurred i.e. I/O operations are available
    bool good() const;
    /// checks if end-of-file has been reached
    bool eof() const;
    /// checks if an error has occurred
    bool fail() const;
    /// checks if a non-recoverable error has occurred
    bool bad() const;
    /// clear state flags
    void clear();

    template <typename Str, typename... Args>
    void print(const Str& format_str, const Args&... args) {
        fmt::print(stream_, format_str, args...);
    }

protected:
    /// Initialize the TextFile at the given `path` and `mode`, with the
    /// specified 'compression' method. All read and write operations will go
    /// through the provided `buffer`.
    TextFile(std::string path, File::Mode mode, File::Compression compression, std::streambuf* buffer);

private:
    void get_line_impl(std::string& string);
    void skip_line_impl();

    std::iostream stream_;
};

} // namespace chemfiles

#endif
