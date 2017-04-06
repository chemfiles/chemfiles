// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

// clang-format off
#ifndef CHEMFILES_CONFIG_HPP
#define CHEMFILES_CONFIG_HPP

/// An integer containing the major (x.0.0) version number
#define CHEMFILES_VERSION_MAJOR @CHEMFILES_VERSION_MAJOR@
/// An integer containing the minor (0.x.0) version number
#define CHEMFILES_VERSION_MINOR @CHEMFILES_VERSION_MINOR@
/// An integer containing the patch (0.0.x) version number
#define CHEMFILES_VERSION_PATCH @CHEMFILES_VERSION_PATCH@
/// The full version of chemfiles, as a string
#define CHEMFILES_VERSION "@CHEMFILES_VERSION@"

// Include the export definitions
#include "chemfiles/exports.hpp"

// clang-format on

#endif
