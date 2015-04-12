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

// Macro to string conversion
#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

#define CHRP_VERSION STR(CHRP_VERSION_MAJOR) "." \
                     STR(CHRP_VERSION_MINOR) "." \
                     STR(CHRP_VERSION_PATCH)

#define HAVE_NETCDF @HAVE_NETCDF@

#endif
