/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

/*! @file Chemharp.hpp
* Chemharp main C++ API header.
*
* This file includes the public API headers for Chemharp, and should be the only
* one included by client applications.
*/

#ifndef CHEMHARP_HPP
#define CHEMHARP_HPP

// This will be used as a protection to prevent internal macro from being exported
#define CHEMHARP_PUBLIC

#include "chemharp/@CHEMHARP_PREFIX@config.hpp"

#include "chemharp/@CHEMHARP_PREFIX@Logger.hpp"
#include "chemharp/@CHEMHARP_PREFIX@Error.hpp"

#include "chemharp/@CHEMHARP_PREFIX@Atom.hpp"
#include "chemharp/@CHEMHARP_PREFIX@Topology.hpp"
#include "chemharp/@CHEMHARP_PREFIX@Frame.hpp"
#include "chemharp/@CHEMHARP_PREFIX@UnitCell.hpp"
#include "chemharp/@CHEMHARP_PREFIX@Trajectory.hpp"

#undef CHEMHARP_PUBLIC

#endif // HARP_HPP
