// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FILES_HPP
#define CHEMFILES_FILES_HPP

#include <string>
#include <vector>

#include <fmt/ostream.h>

#include "chemfiles/exports.h"
#include "chemfiles/string_view.hpp"
#include "chemfiles/ErrorFmt.hpp"

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
    File(const File&) = delete;
    File& operator=(const File&) = delete;

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

/// Abstract base class for readers used by text files. This is specialized for
/// compressed files, and might get extended to network or memory mapped files.
class TextFileImpl {
public:
    TextFileImpl() = default;
    virtual ~TextFileImpl() = default;
    TextFileImpl(TextFileImpl&&) = delete;
    TextFileImpl(const TextFileImpl&) = delete;
    TextFileImpl& operator=(TextFileImpl&&) = delete;
    TextFileImpl& operator=(const TextFileImpl&) = delete;

    /// clear error and eof flags on the file
    virtual void clear() = 0;

    /// Set file indicator to `position` characters after the start of the file
    virtual void seek(int64_t position) = 0;

    /// Fill up the `data` buffer reading at most `count` characters from the
    /// file.
    ///
    /// Returns the amount of characters read. A return value of 0 indicates
    /// that the end of file has been reached.
    virtual size_t read(char* data, size_t count) = 0;

    /// Write `count` characters starting at `data` to the file. Returns the
    /// number of character actually writen.
    virtual size_t write(const char* data, size_t count) = 0;
};

/// Abstract base class representing a text file. This class is inteded to be
/// inherited by any form of text files: compressed files, memory-mapped files,
/// etc.
///
/// All failling operations should throw a `FileError` instead of waiting for
/// the user of the class to the current state.
class CHFL_EXPORT TextFile final: public File {
public:
    /// Open the file at the given `path` with the requested `mode` and
    /// `compression` method, picking the best `TextFileImpl`.
    TextFile(std::string path, File::Mode mode, File::Compression compression);

    TextFile(TextFile&&) = default;
    TextFile& operator=(TextFile&&) = default;
    TextFile(const TextFile&) = delete;
    TextFile& operator=(const TextFile&) = delete;

    /// returns the current position indicator
    int64_t tellpos() const;

    /// set the position indicator to `position`
    void seekpos(int64_t position);

    /// Reset the position indicator to the beggining of the file
    void rewind();

    /// checks if end-of-file has been reached
    bool eof() const {
        return eof_;
    }

    /// clear eof flags on the file
    void clear();

    /// Read a line from the file
    string_view readline();

    /// Read the full file into a string.
    std::string readall();

    template <typename Str, typename... Args>
    void print(const Str& format, const Args&... args) {
        this->vprint(format, fmt::make_format_args(args...));
    }

private:
    /// Fill the buffer, calling `refill` and setting all needed internal values
    void fill_buffer(size_t start);

    /// Actually format and print data to the file
    void vprint(fmt::string_view format, fmt::format_args args);

    bool buffer_initialized() const;

    /// Pointer to the actual file implementation
    std::unique_ptr<TextFileImpl> file_;
    /// Buffer to store read characters
    std::vector<char> buffer_;
    /// Start of the current line;
    const char* line_start_;
    /// End of the buffer
    const char* end_;
    /// Current position in the file, this points to the character
    /// corresponding to the start of the buffer
    int64_t position_ = 0;
    /// Did we reach the end of the underlying buffer? Since we are
    /// buffering data, this does not necessarly correspond to `this->eof()`;
    bool got_impl_eof_ = false;
    /// Did we actually reached the end of file while reading a line?
    bool eof_ = false;
};

} // namespace chemfiles

#endif
