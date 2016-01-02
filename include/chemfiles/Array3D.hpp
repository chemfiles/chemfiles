/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, rhs. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_VECTOR3D_HPP
#define CHEMFILES_VECTOR3D_HPP

#include <vector>
#include <array>
#include <iostream>
#include <cmath>
#include <cassert>

namespace chemfiles {

typedef std::array<float, 3> Vector3D;

/// Create a Vector3D from the three components \c x, \c y, and \c z.
inline Vector3D vector3d(float x, float y, float z) {
    return Vector3D{{x, y, z}};
}

inline std::ostream& operator<<(std::ostream& out, const Vector3D& rhs){
    out << rhs[0] << ", " << rhs[1] << ", " << rhs[2];
    return out;
}

inline bool operator==(const Vector3D& lhs, const Vector3D& rhs){
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

//! Compute the dot product of the vectors `lhs` and `rhs`.
inline double dot(const Vector3D& lhs, const Vector3D& rhs) {
    return lhs[0]*rhs[0] + lhs[1]*rhs[1] + lhs[2]*rhs[2];
}

//! Compute the cross product of the vectors `lhs` and `rhs`.
inline Vector3D cross(const Vector3D& lhs, const Vector3D& rhs) {
    auto x = lhs[1]*rhs[2] - lhs[2]*rhs[1];
    auto y = lhs[2]*rhs[0] - lhs[0]*rhs[2];
    auto z = lhs[0]*rhs[1] - lhs[1]*rhs[0];
    return vector3d(x, y, z);
}

//! Compute the squared euclidean norm of a vector.
inline double norm2(const Vector3D& rhs) {
    return dot(rhs, rhs);
}

//! Compute the euclidean norm of a vector.
inline double norm(const Vector3D& rhs) {
    return std::sqrt(norm2(rhs));
}

//! Add two vectors
inline Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs){
    return std::array<float, 3>{{lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]}};
}

//! Substract two vectors
inline Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs){
    return std::array<float, 3>{{lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]}};
}

//! Multiply a vector by a scalar on the right
inline Vector3D operator*(const Vector3D& lhs, float rhs){
    return vector3d(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs);
}

//! Multiply a vector by a scalar on the left
inline Vector3D operator*(float lhs, const Vector3D& rhs){
    return vector3d(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
}

//! Divide a vector by a scalar
inline Vector3D operator/(const Vector3D& lhs, float rhs){
    return vector3d(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs);
}

// As `std::array<float, 3>` (i.e. Vector3D) is POD, its memory layout is
// equivalent to a `float[3]` array. So the pointer return by
// `std::vector<Vector3D>::data` is compatible with the C type `(*float)[3]`
using Array3D = std::vector<Vector3D>;

} // namespace chemfiles

#endif
