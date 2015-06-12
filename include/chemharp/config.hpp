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

#define CHRP_VERSION_MAJOR 0
#define CHRP_VERSION_MINOR 2
#define CHRP_VERSION_PATCH 0
#define CHRP_VERSION "0.2.0"

// Include the MSVC export definitions
#include "chemharp/exports.hpp"

// The CHEMHARP_PUBLIC macro should be defined when including this file to prevent
// unwanted macros from being exported.
#ifndef CHEMHARP_PUBLIC
    #define HAVE_NETCDF 1
#endif // CHEMHARP_PUBLIC

#endif
