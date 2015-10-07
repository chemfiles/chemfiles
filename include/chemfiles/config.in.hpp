/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_CONFIG_HPP
#define CHEMFILES_CONFIG_HPP

#define CHEMFILES_VERSION_MAJOR @CHEMFILES_VERSION_MAJOR@
#define CHEMFILES_VERSION_MINOR @CHEMFILES_VERSION_MINOR@
#define CHEMFILES_VERSION_PATCH @CHEMFILES_VERSION_PATCH@
#define CHEMFILES_VERSION_SHORT "@CHEMFILES_VERSION_SHORT@"
#define CHEMFILES_VERSION "@CHEMFILES_VERSION@"

// Include the export definitions
#include "chemfiles/exports.hpp"

// Are we compiling for Windows ?
#if defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __CYGWIN__ ) || \
    defined( WIN64 ) || defined( _WIN64 ) || defined( __WIN64__ )
    #define CHFL_WINDOWS
#endif

// The CHEMFILES_PUBLIC macro should be defined when including this file to prevent
// unwanted macros from being exported.
#ifndef CHEMFILES_PUBLIC
    #define HAVE_NETCDF @HAVE_NETCDF@
#endif // CHEMFILES_PUBLIC

#endif
