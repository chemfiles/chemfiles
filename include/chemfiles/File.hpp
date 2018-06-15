// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FILES_HPP
#define CHEMFILES_FILES_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "chemfiles/exports.hpp"

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

    virtual ~File() noexcept = default;
    File(File&&) = default;
    File& operator=(File&&) = delete;
    File(File const&) = delete;
    File& operator=(File const&) = delete;

    /// File name, i.e. complete path to this file on disk.
    const std::string& filename() const { return filename_; }
    /// File opening mode.
    Mode mode() const { return mode_; }

protected:
    File(std::string path, Mode mode): filename_(std::move(path)), mode_(mode) {}

private:
    const std::string filename_;
    Mode mode_;
};

/// Abstract base class representing a text file. This class is inteded to be
/// inherited by any form of text files: compressed files, memory-mapped files,
/// etc.
///
/// All failling operations should throw a `FileError` instead of waiting for
/// the user of the class to the current state.
class CHFL_EXPORT TextFile: public File, public std::iostream {
public:
    /// Open the most adaptated text file class for the given `path` and `mode`
    static std::unique_ptr<TextFile> create(const std::string& path, File::Mode mode);

    /// Read a line from the file
    std::string readline();
    /// Read `n` lines from the file
    std::vector<std::string> readlines(size_t n);
    /// Reset the file cursor
    void rewind();
    /// Are we at the end of the file ?
    bool eof();

protected:
    /// Initialize the TextFile at the given `path` and `mode`. All read and
    /// write operations will go through the provided `buffer`.
    TextFile(const std::string& path, File::Mode mode, std::streambuf* buffer);

private:
    void get_line(std::string& string);
};

} // namespace chemfiles

#endif
