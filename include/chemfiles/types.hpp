/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, rhs. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_TYPES_HPP
#define CHEMFILES_TYPES_HPP

#include <type_traits>
#include <array>
#include <cassert>
#include <cmath>
#include <vector>

#include "span.hpp"

namespace chemfiles {

typedef std::array<double, 3> Vector3D;

/// Create a Vector3D from the three components `x`, `y`, and `z`.
inline Vector3D vector3d(double x, double y, double z) {
    return Vector3D{{x, y, z}};
}

inline bool operator==(const Vector3D& lhs, const Vector3D& rhs) {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

//! Compute the dot product of the vectors `lhs` and `rhs`.
inline double dot(const Vector3D& lhs, const Vector3D& rhs) {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

//! Compute the cross product of the vectors `lhs` and `rhs`.
inline Vector3D cross(const Vector3D& lhs, const Vector3D& rhs) {
    auto x = lhs[1] * rhs[2] - lhs[2] * rhs[1];
    auto y = lhs[2] * rhs[0] - lhs[0] * rhs[2];
    auto z = lhs[0] * rhs[1] - lhs[1] * rhs[0];
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
inline Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs) {
    return {{lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]}};
}

//! Substract two vectors
inline Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs) {
    return {{lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]}};
}

//! Multiply a vector by a scalar on the right
inline Vector3D operator*(const Vector3D& lhs, double rhs) {
    return {{lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs}};
}

//! Multiply a vector by a scalar on the left
inline Vector3D operator*(double lhs, const Vector3D& rhs) {
    return {{lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]}};
}

//! Divide a vector by a scalar
inline Vector3D operator/(const Vector3D& lhs, double rhs) {
    return {{lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs}};
}

// As `std::array<double, 3>` (i.e. Vector3D) is POD, its memory layout is
// equivalent to a `double[3]` array. So the pointer return by `Array3D::data`
// is compatible with the C type `(*double)[3] == chfl_vector_t`.
static_assert(std::is_pod<Vector3D>::value, "Vector3D must be POD");

//! A vector of `Vector3D`, used as a list of positions or velocities in a
//! system.
using Array3D = std::vector<Vector3D>;

//! A Span3D is a view into an array of `Vector3D`, usually an `Array3D`. This
//! view can mutate the memory and modify the `Vector3D`, but not change the
//! size of the array.
using Span3D = span<Vector3D>;

//! 3 x 3 matrix type
using Matrix3D = std::array<std::array<double, 3>, 3>;

//! Create a diagonal Matrix3D with the three diagonal elements `a`, `b` and `c`
inline Matrix3D matrix3d(double a, double b, double c) {
    return Matrix3D{{
        {{a, 0, 0}},
        {{0, b, 0}},
        {{0, 0, c}},
    }};
}

//! Create a Matrix3d filled with zeros.
inline Matrix3D matrix3d() {
    return matrix3d(0, 0, 0);
}

//! Multiplication of a vector by a matrix
inline Vector3D operator*(const Matrix3D& lhs, const Vector3D& rhs) {
    return {{lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2],
             lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2],
             lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2]}};
}

//! Multiplication of two matrix
inline Matrix3D operator*(const Matrix3D& lhs, const Matrix3D& rhs) {
    auto res = matrix3d();
    res[0][0] = lhs[0][0] * rhs[0][0] + lhs[0][1] * rhs[1][0] + lhs[0][2] * rhs[2][0];
    res[1][0] = lhs[1][0] * rhs[0][0] + lhs[1][1] * rhs[1][0] + lhs[1][2] * rhs[2][0];
    res[2][0] = lhs[2][0] * rhs[0][0] + lhs[2][1] * rhs[1][0] + lhs[2][2] * rhs[2][0];

    res[0][1] = lhs[0][0] * rhs[0][1] + lhs[0][1] * rhs[1][1] + lhs[0][2] * rhs[2][1];
    res[1][1] = lhs[1][0] * rhs[0][1] + lhs[1][1] * rhs[1][1] + lhs[1][2] * rhs[2][1];
    res[2][1] = lhs[2][0] * rhs[0][1] + lhs[2][1] * rhs[1][1] + lhs[2][2] * rhs[2][1];

    res[0][2] = lhs[0][0] * rhs[0][2] + lhs[0][1] * rhs[1][2] + lhs[0][2] * rhs[2][2];
    res[1][2] = lhs[1][0] * rhs[0][2] + lhs[1][1] * rhs[1][2] + lhs[1][2] * rhs[2][2];
    res[2][2] = lhs[2][0] * rhs[0][2] + lhs[2][1] * rhs[1][2] + lhs[2][2] * rhs[2][2];
    return res;
}

//! Compute the inverse of an invertible matrix. The matrix `A` must be
//! invertible.
inline Matrix3D invert(const Matrix3D& A) {
    double determinant = 0.0;
    determinant += A[0][0] * (A[1][1] * A[2][2] - A[2][1] * A[1][2]);
    determinant -= A[0][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0]);
    determinant += A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);;

    assert(determinant != 0.0 && "The matrix is not inversible!");
    auto invdet = 1.0 / determinant;
    Matrix3D res;
    res[0][0] = (A[1][1] * A[2][2] - A[2][1] * A[1][2]) * invdet;
    res[0][1] = (A[0][2] * A[2][1] - A[0][1] * A[2][2]) * invdet;
    res[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) * invdet;
    res[1][0] = (A[1][2] * A[2][0] - A[1][0] * A[2][2]) * invdet;
    res[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]) * invdet;
    res[1][2] = (A[1][0] * A[0][2] - A[0][0] * A[1][2]) * invdet;
    res[2][0] = (A[1][0] * A[2][1] - A[2][0] * A[1][1]) * invdet;
    res[2][1] = (A[2][0] * A[0][1] - A[0][0] * A[2][1]) * invdet;
    res[2][2] = (A[0][0] * A[1][1] - A[1][0] * A[0][1]) * invdet;
    return res;
}

} // namespace chemfiles

#endif
