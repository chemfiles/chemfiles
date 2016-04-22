/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

// clang-format off
#ifndef CHEMFILES_CONFIG_HPP
#define CHEMFILES_CONFIG_HPP

//! An integer containing the major (x.0.0) version number
#define CHEMFILES_VERSION_MAJOR @CHEMFILES_VERSION_MAJOR@
//! An integer containing the minor (0.x.0) version number
#define CHEMFILES_VERSION_MINOR @CHEMFILES_VERSION_MINOR@
//! An integer containing the patch (0.0.x) version number
#define CHEMFILES_VERSION_PATCH @CHEMFILES_VERSION_PATCH@
//! The full version of chemfiles, as a string
#define CHEMFILES_VERSION "@CHEMFILES_VERSION@"

// Include the export definitions
#include "chemfiles/exports.hpp"

// The CHEMFILES_PUBLIC macro should be defined when including this file to
// prevent unwanted macros from being exported.
#ifndef CHEMFILES_PUBLIC
// Are we compiling for Windows ?
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||                 \
    defined(__CYGWIN__) || defined(WIN64) || defined(_WIN64) ||                \
    defined(__WIN64__)
#define CHFL_WINDOWS
#endif

#define HAVE_NETCDF @HAVE_NETCDF@
#endif // CHEMFILES_PUBLIC
// clang-format on

#endif
