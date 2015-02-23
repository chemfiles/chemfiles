/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_VECTOR3D_HPP
#define HARP_VECTOR3D_HPP

#include <array>
#include <vector>

//! Fixed-size array of 3 components: x, y and z values.
typedef std::array<float, 3> Vector3D;
//! Variable-size array of vector of 3 components
typedef std::vector<Vector3D> Array3D;

#define Vector3D(x, y, z) (Vector3D{{(x), (y), (z)}})

#endif
