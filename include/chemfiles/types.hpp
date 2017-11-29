// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_TYPES_HPP
#define CHEMFILES_TYPES_HPP

#include <type_traits>
#include <array>
#include <cfloat>
#include <cmath>

#include <chemfiles/Error.hpp>

namespace chemfiles {

/// 3D vector for basic data storage in chemfiles.
///
/// This type defines the following operators, with the usual meaning:
///
/// * Comparison operators: `==` and `!=` performs strict float equality
///   comparison;
/// * Mathematical operators: `+` and `-` for addition and substraction of
///   vectors, `*` and `/` for multiplication and division by double values.
///   Multiplication with `*` is also defined for multiplication by a
///   `Matrix3D`.
///
/// @example{tests/doc/vector3d/ops.cpp}
class Vector3D final: private std::array<double, 3> {
    using super = std::array<double, 3>;
public:
    /// Create a Vector3D with all components equal to 0.
    ///
    /// @example{tests/doc/vector3d/vector3d-0.cpp}
    Vector3D(): Vector3D(0, 0, 0) {}

    /// Create a Vector3D from the three components `x`, `y`, and `z`.
    ///
    /// @example{tests/doc/vector3d/vector3d-3.cpp}
    Vector3D(double x, double y, double z): super({{x, y, z}}) {}

    ~Vector3D() = default;
    Vector3D(const Vector3D&) = default;
    Vector3D& operator=(const Vector3D&) = default;
    Vector3D(Vector3D&&) = default;
    Vector3D& operator=(Vector3D&&) = default;

    using super::operator[];

    /// Compute the euclidean norm of this Vector3D.
    ///
    /// @example{tests/doc/vector3d/norm.cpp}
    double norm() const;
};

/// Compute the dot product of the vectors `lhs` and `rhs`.
///
/// @example{tests/doc/vector3d/dot.cpp}
inline double dot(const Vector3D& lhs, const Vector3D& rhs) {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

/// Compute the cross product of the vectors `lhs` and `rhs`.
///
/// @example{tests/doc/vector3d/cross.cpp}
inline Vector3D cross(const Vector3D& lhs, const Vector3D& rhs) {
    auto x = lhs[1] * rhs[2] - lhs[2] * rhs[1];
    auto y = lhs[2] * rhs[0] - lhs[0] * rhs[2];
    auto z = lhs[0] * rhs[1] - lhs[1] * rhs[0];
    return {x, y, z};
}

inline double Vector3D::norm() const {
    return std::sqrt(dot(*this, *this));
}

/// Compare two vectors for equality using float equality
inline bool operator==(const Vector3D& lhs, const Vector3D& rhs) {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

/// Compare two vectors for inequality using float equality
inline bool operator!=(const Vector3D& lhs, const Vector3D& rhs) {
    return !(lhs == rhs);
}

/// Add two vectors
inline Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs) {
    return {lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]};
}

/// Substract two vectors
inline Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs) {
    return {lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]};
}

/// Multiply a vector by a scalar on the right
inline Vector3D operator*(const Vector3D& lhs, double rhs) {
    return {lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs};
}

/// Multiply a vector by a scalar on the left
inline Vector3D operator*(double lhs, const Vector3D& rhs) {
    return {lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]};
}

/// Divide a vector by a scalar
inline Vector3D operator/(const Vector3D& lhs, double rhs) {
    return {lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs};
}

// Vector3D needs to have a standard layout, equivalent to a `double[3]` array.
// This means that the pointer return by `std::vector<Vector3D>::data` is
// compatible with the `chfl_vector3d` type (`double[3]`).
static_assert(std::is_standard_layout<Vector3D>::value, "Vector3D must have a standard layout");

/// A 3x3 matrix class.
///
/// This type defines the following operators, with the usual meaning:
///
/// * Comparison operators: `==` and `!=` performs strict float equality
///   comparison;
/// * Mathematical operators: only mulitplication between matrixes and between
///   matrix and vector are defined, using the `*` operator.
///
/// @example{tests/doc/matrix3d/ops.cpp}
class Matrix3D final: private std::array<std::array<double, 3>, 3> {
    using super = std::array<std::array<double, 3>, 3>;
public:
    /// Create a Matrix3D with all components equal to 0
    ///
    /// @example{tests/doc/matrix3d/matrix3d-0.cpp}
    Matrix3D(): Matrix3D(0, 0, 0) {}

    /// Create a diagonal Matrix3D with the three diagonal elements `a`, `b` and
    /// `c`.
    ///
    /// @example{tests/doc/matrix3d/matrix3d-3.cpp}
    Matrix3D(double a, double b, double c): Matrix3D(a, 0, 0, 0, b, 0, 0, 0, c) {}

    /// Create a `Matrix3D` by explicitly specifying all the `m_ij` components
    /// of the matrix.
    ///
    /// @example{tests/doc/matrix3d/matrix3d-9.cpp}
    Matrix3D(double m11, double m12, double m13,
             double m21, double m22, double m23,
             double m31, double m32, double m33):
        super({{ {{m11, m12, m13}}, {{m21, m22, m23}}, {{m31, m32, m33}} }}) {}

    ~Matrix3D() = default;
    Matrix3D(const Matrix3D&) = default;
    Matrix3D& operator=(const Matrix3D&) = default;
    Matrix3D(Matrix3D&&) = default;
    Matrix3D& operator=(Matrix3D&&) = default;

    using super::operator[];

    /// Compute the inverse of this matrix.
    ///
    /// @throw Error if the matrix is not inversible
    /// @example{tests/doc/matrix3d/invert.cpp}
    Matrix3D invert() const;
};

/// Compare two matrix for equality using float equality on all components
inline bool operator==(const Matrix3D& lhs, const Matrix3D& rhs) {
    return lhs[0][0] == rhs[0][0] && lhs[1][0] == rhs[1][0] && lhs[2][0] == rhs[2][0] &&
           lhs[0][1] == rhs[0][1] && lhs[1][1] == rhs[1][1] && lhs[2][1] == rhs[2][1] &&
           lhs[0][2] == rhs[0][2] && lhs[1][2] == rhs[1][2] && lhs[2][2] == rhs[2][2];
}

/// Compare two matrix for non-equality using float equality on all components
inline bool operator!=(const Matrix3D& lhs, const Matrix3D& rhs) {
    return !(lhs == rhs);
}

/// Multiplication of a vector by a matrix
inline Vector3D operator*(const Matrix3D& lhs, const Vector3D& rhs) {
    return {
        lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2],
        lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2],
        lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2]
    };
}

/// Multiplication of two matrix
inline Matrix3D operator*(const Matrix3D& lhs, const Matrix3D& rhs) {
    Matrix3D res;
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

inline Matrix3D Matrix3D::invert() const {
    double determinant = 0.0;
    determinant += (*this)[0][0] * ((*this)[1][1] * (*this)[2][2] - (*this)[2][1] * (*this)[1][2]);
    determinant -= (*this)[0][1] * ((*this)[1][0] * (*this)[2][2] - (*this)[1][2] * (*this)[2][0]);
    determinant += (*this)[0][2] * ((*this)[1][0] * (*this)[2][1] - (*this)[1][1] * (*this)[2][0]);;

    if (determinant <= DBL_EPSILON) {
        throw Error("can not call invert on this matrix. This is likely to be a bug");
    }
    auto invdet = 1.0 / determinant;
    Matrix3D res;
    res[0][0] = ((*this)[1][1] * (*this)[2][2] - (*this)[2][1] * (*this)[1][2]) * invdet;
    res[0][1] = ((*this)[0][2] * (*this)[2][1] - (*this)[0][1] * (*this)[2][2]) * invdet;
    res[0][2] = ((*this)[0][1] * (*this)[1][2] - (*this)[0][2] * (*this)[1][1]) * invdet;
    res[1][0] = ((*this)[1][2] * (*this)[2][0] - (*this)[1][0] * (*this)[2][2]) * invdet;
    res[1][1] = ((*this)[0][0] * (*this)[2][2] - (*this)[0][2] * (*this)[2][0]) * invdet;
    res[1][2] = ((*this)[1][0] * (*this)[0][2] - (*this)[0][0] * (*this)[1][2]) * invdet;
    res[2][0] = ((*this)[1][0] * (*this)[2][1] - (*this)[2][0] * (*this)[1][1]) * invdet;
    res[2][1] = ((*this)[2][0] * (*this)[0][1] - (*this)[0][0] * (*this)[2][1]) * invdet;
    res[2][2] = ((*this)[0][0] * (*this)[1][1] - (*this)[1][0] * (*this)[0][1]) * invdet;
    return res;
}

} // namespace chemfiles

#endif
