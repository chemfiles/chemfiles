/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_CONFIG_HPP
#define HARP_CONFIG_HPP

#define CHRP_VERSION_MAJOR @CHRP_VERSION_MAJOR@
#define CHRP_VERSION_MINOR @CHRP_VERSION_MINOR@
#define CHRP_VERSION_PATCH @CHRP_VERSION_PATCH@
#define CHRP_VERSION "@CHRP_VERSION@"

// Include the export definitions
#include "chemharp/@CHEMHARP_PREFIX@exports.hpp"

// Are we compiling for Windows ?
#if defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __CYGWIN__ ) || \
    defined( WIN64 ) || defined( _WIN64 ) || defined( __WIN64__ )
    #define CHRP_WINDOWS
#endif

// The CHEMHARP_PUBLIC macro should be defined when including this file to prevent
// unwanted macros from being exported.
#ifndef CHEMHARP_PUBLIC
    #define HAVE_NETCDF @HAVE_NETCDF@
#endif // CHEMHARP_PUBLIC

#endif
