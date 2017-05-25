// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_ERROR_HPP
#define CHEMFILES_ERROR_HPP

#include <stdexcept>
#include <string>
#include "chemfiles/exports.hpp"

namespace chemfiles {

/// @brief Base exception class for chemfiles library
struct CHFL_EXPORT Error : public std::runtime_error {
    Error(const std::string& err) : std::runtime_error(err) {}
};

/// @brief Exception for files related failures
struct CHFL_EXPORT FileError : public Error {
    FileError(const std::string& err) : Error(err) {}
};

/// @brief Exception for memory related failures
struct CHFL_EXPORT MemoryError : public Error {
    MemoryError(const std::string& err) : Error(err) {}
};

/// @brief Exception for formats related failures
struct CHFL_EXPORT FormatError : public Error {
    FormatError(const std::string& err) : Error(err) {}
};

/// @brief Exception for errors in selections
struct CHFL_EXPORT SelectionError : public Error {
    SelectionError(const std::string& err) : Error(err) {}
};

/// @brief Exception for errors in configuration files
struct CHFL_EXPORT ConfigurationError : public Error {
    ConfigurationError(const std::string& err) : Error(err) {}
};

} // namespace chemfiles

#endif
