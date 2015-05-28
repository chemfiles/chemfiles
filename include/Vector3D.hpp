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
#include <iostream>
#include <cmath>

namespace harp {

//! Fixed-size array of 3 components: x, y and z values.
class Vector3D : private std::array<float, 3> {
public:
    Vector3D() : Vector3D(0, 0, 0) {}
    Vector3D(float x, float y, float z){
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
    }

    Vector3D(const Vector3D&) = default;
    Vector3D(Vector3D&&) = default;
    Vector3D& operator=(const Vector3D&) = default;
    Vector3D& operator=(Vector3D&&) = default;
    ~Vector3D() = default;

    using std::array<float, 3>::operator[];
};

inline bool operator==(const Vector3D& u, const Vector3D& v){
    return u[0] == v[0] && u[1] == v[1] && u[2] == v[2];
}

inline std::ostream& operator<<(std::ostream& out, const Vector3D& v){
    out << v[0] << ", " << v[1] << ", " << v[2];
    return out;
}

//! Compute the dot product of the vectors \c u and \c v.
inline double dot(const Vector3D& v, const Vector3D& u) {
    return v[0]*u[0] + v[1]*u[1] + v[2]*u[2];
}

//! Compute the squared euclidean norm of a vector.
inline double norm2(const Vector3D& v) {
    return dot(v, v);
}

//! Compute the euclidean norm of a vector.
inline double norm(const Vector3D& v) {
    return std::sqrt(norm2(v));
}

//! Variable-size array of vector of 3 components
typedef std::vector<Vector3D> Array3D;

}
#endif
