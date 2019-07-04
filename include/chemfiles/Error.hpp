// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_ERROR_HPP
#define CHEMFILES_ERROR_HPP

#include <stdexcept>
#include <string>
#include "chemfiles/exports.h"

namespace chemfiles {

/// Base exception class for chemfiles library
struct CHFL_EXPORT Error: public std::runtime_error {
    Error(const std::string& err): std::runtime_error(err) {}
};

/// Exception for files related failures
struct CHFL_EXPORT FileError: public Error {
    FileError(const std::string& err): Error(err) {}
};

/// Exception for memory related failures
struct CHFL_EXPORT MemoryError: public Error {
    MemoryError(const std::string& err): Error(err) {}
};

/// Exception for formats related failures
struct CHFL_EXPORT FormatError: public Error {
    FormatError(const std::string& err): Error(err) {}
};

/// Exception for errors in selections
struct CHFL_EXPORT SelectionError: public Error {
    SelectionError(const std::string& err): Error(err) {}
};

/// Exception for errors in configuration files
struct CHFL_EXPORT ConfigurationError: public Error {
    ConfigurationError(const std::string& err): Error(err) {}
};

/// Exception for out of bounds error when accessing atoms or residues
struct CHFL_EXPORT OutOfBounds: public Error {
    OutOfBounds(const std::string& err): Error(err) {}
};

/// Exception for errors related to frame and atomic properties
struct CHFL_EXPORT PropertyError: public Error {
    PropertyError(const std::string& err): Error(err) {}
};

} // namespace chemfiles

#endif
