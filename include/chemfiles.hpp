/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

/*! @file chemfiles.hpp
* Chemfiles main C++ API header.
*
* This file includes the public API headers for chemfiles, and should be the only
* one included by client applications.
*/

#ifndef CHEMFILES_HPP
#define CHEMFILES_HPP

// This will be used as a protection to prevent internal macro from being exported
#define CHEMFILES_PUBLIC

#include "chemfiles/config.hpp"

#include "chemfiles/Logger.hpp"
#include "chemfiles/Error.hpp"

#include "chemfiles/Atom.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Trajectory.hpp"

#undef CHEMFILES_PUBLIC

#endif // CHEMFILES_HPP
