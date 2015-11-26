/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
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

//! View of size 3 into an Array3D. As a view, this class do not own its memory, and
//! can be invalidated by any resize happening in the owing Array3D, or by the
//! corresponding Array3D going out of scope.
//!
//! A View3D is trivially convertible to a Vector3D, and this conversion happen every
//! time an operator is called on Vector3D (+, -, *, /).
template<typename T> class View3D {
public:
    //! Indexing operation, with bound checking in debug mode.
    T& operator[](std::size_t i) {
        assert(i < 3 && "Index is out of bounds in Vector3D");
        return *(data + i);
    }
    //! Indexing operation, with bound checking in debug mode.
    const T& operator[](std::size_t i) const {
        assert(i < 3 && "Index is out of bounds in Vector3D");
        return *(data + i);
    }

    /// Construct a Vector3D from a pointer to the data. This pointer MUST be valid and
    /// point to the first element of a 3-dimmensional array of float.
    View3D(T* data) : data(data) {
        assert(data != nullptr);
    }

    /// Convert this view into an owning std::array.
    operator Vector3D() {
        return Vector3D{{data[0], data[1], data[2]}};
    }

    View3D(View3D&&) = default;
    View3D& operator=(View3D&&) = default;
    View3D& operator=(const Vector3D& other) {
        (*this)[0] = other[0];
        (*this)[1] = other[1];
        (*this)[2] = other[2];
        return *this;
    };

    View3D& operator=(const View3D&) = delete;
    View3D(const View3D&) = delete;

    ~View3D() = default;
private:
    T* data;
};

template <typename T> struct is_vector_3d {
     static constexpr bool value = false;
};

template <> struct is_vector_3d<View3D<float>> {
     static constexpr bool value = true;
};

template <> struct is_vector_3d<View3D<const float>> {
     static constexpr bool value = true;
};

template <> struct is_vector_3d<Vector3D> {
     static constexpr bool value = true;
};

template<class T>
inline std::ostream& operator<<(std::ostream& out, const View3D<T>& v){
    out << v[0] << ", " << v[1] << ", " << v[2];
    return out;
}

template<class T, class S,
         class = typename std::enable_if<is_vector_3d<T>::value>::type,
         class = typename std::enable_if<is_vector_3d<S>::value>::type>
inline bool operator==(const T& u, const S& v){
    return u[0] == v[0] && u[1] == v[1] && u[2] == v[2];
}

//! Compute the dot product of the vectors \c u and \c v.
template<class T, class = typename std::enable_if<is_vector_3d<T>::value>::type>
inline double dot(const T& v, const T& u) {
    return v[0]*u[0] + v[1]*u[1] + v[2]*u[2];
}

//! Compute the squared euclidean norm of a vector.
template<class T, class = typename std::enable_if<is_vector_3d<T>::value>::type>
inline double norm2(const T& v) {
    return dot(v, v);
}

//! Compute the euclidean norm of a vector.
template<class T, class = typename std::enable_if<is_vector_3d<T>::value>::type>
inline double norm(const T& v) {
    return std::sqrt(norm2(v));
}

template<class T, class S,
    class = typename std::enable_if<is_vector_3d<T>::value>::type,
    class = typename std::enable_if<is_vector_3d<S>::value>::type>
inline std::array<float, 3> operator+(const T& u, const S& v){
    return std::array<float, 3>{{u[0] + v[0], u[1] + v[1], u[2] + v[2]}};
}

template<class T, class S,
    class = typename std::enable_if<is_vector_3d<T>::value>::type,
    class = typename std::enable_if<is_vector_3d<S>::value>::type>
inline std::array<float, 3> operator-(const T& u, const S& v){
    return std::array<float, 3>{{u[0] - v[0], u[1] - v[1], u[2] - v[2]}};
}

template<class T, class = typename std::enable_if<is_vector_3d<T>::value>::type>
inline std::array<float, 3> operator*(const T& u, float a){
    return std::array<float, 3>{{u[0] * a, u[1] * a, u[2] * a}};
}

template<class T, class = typename std::enable_if<is_vector_3d<T>::value>::type>
inline std::array<float, 3> operator*(float a, const T& v){
    return std::array<float, 3>{{a * v[0], a * v[1], a * v[2]}};
}

template<class T, class = typename std::enable_if<is_vector_3d<T>::value>::type>
inline std::array<float, 3> operator/(const T& u, float a){
    return std::array<float, 3>{{u[0] / a, u[1] / a, u[2] / a}};
}


//! TODO
class Array3D: private std::vector<float> {
    using super = std::vector<float>;
public:
    Array3D() : Array3D(0) {}
    Array3D(std::size_t n) : Array3D(n, 0.0) {}
    Array3D(std::size_t n, float value) : super(3 * n, value) {}

    Array3D& operator=(const Array3D&) = default;
    Array3D(const Array3D&) = default;

    Array3D(Array3D&&) = default;
    Array3D& operator=(Array3D&&) = default;

    ~Array3D() = default;

    View3D<float> operator[](std::size_t i) {
        return View3D<float>(super::data() + 3 * i);
    }

    View3D<const float> operator[](std::size_t i) const {
        return View3D<const float>(super::data() + 3*i);
    }

    using std::vector<float>::data;
    using std::vector<float>::empty;

    void resize(std::size_t n) {
        super::resize(3 * n);
    }

    void resize(std::size_t n, float value) {
        super::resize(3 * n, value);
    }

    std::size_t size() const {
        return super::size() / 3;
    }

    std::size_t capacity() const {
        return super::capacity() / 3;
    }
};

} // namespace chemfiles

#endif
