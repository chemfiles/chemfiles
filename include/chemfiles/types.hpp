// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_TYPES_HPP
#define CHEMFILES_TYPES_HPP

#include <type_traits>
#include <array>
#include <cassert>
#include <cmath>
#include <vector>

namespace chemfiles {

/// 3D vector for basic data storage in chemfiles.
///
/// This type defines the following operators, with the usual meaning:
///
/// * Comparison operators: `==` and `!=` performs strict float equality
///   comparison;
/// * Mathematical operators: + and - for addition and substraction of vectors,
///   * and / for multiplication and division by double values. Multiplication
///   with * is also defined for multiplication by a `Matrix3D`.
class Vector3D final: private std::array<double, 3> {
    using super = std::array<double, 3>;
public:
    /// Create a Vector3D with all components equal to 0
    Vector3D(): Vector3D(0, 0, 0) {}

    /// Create a Vector3D from the three components `x`, `y`, and `z`.
    Vector3D(double x, double y, double z): super({{x, y, z}}) {}

    Vector3D(const Vector3D&) = default;
    Vector3D& operator=(const Vector3D&) = default;
    Vector3D(Vector3D&&) = default;
    Vector3D& operator=(Vector3D&&) = default;

    using super::operator[];

    /// Compute the euclidean norm of this Vector3D.
    double norm() const;
};

/// Compute the dot product of the vectors `lhs` and `rhs`.
inline double dot(const Vector3D& lhs, const Vector3D& rhs) {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

/// Compute the cross product of the vectors `lhs` and `rhs`.
inline Vector3D cross(const Vector3D& lhs, const Vector3D& rhs) {
    auto x = lhs[1] * rhs[2] - lhs[2] * rhs[1];
    auto y = lhs[2] * rhs[0] - lhs[0] * rhs[2];
    auto z = lhs[0] * rhs[1] - lhs[1] * rhs[0];
    return Vector3D(x, y, z);
}

/// Compute the cross product of the vectors `lhs` and `rhs`.
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
    return Vector3D(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

/// Substract two vectors
inline Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs) {
    return Vector3D(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]);
}

/// Multiply a vector by a scalar on the right
inline Vector3D operator*(const Vector3D& lhs, double rhs) {
    return Vector3D(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs);
}

/// Multiply a vector by a scalar on the left
inline Vector3D operator*(double lhs, const Vector3D& rhs) {
    return Vector3D(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
}

/// Divide a vector by a scalar
inline Vector3D operator/(const Vector3D& lhs, double rhs) {
    return Vector3D(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs);
}

// As `std::array<double, 3>` (i.e. Vector3D) is POD, its memory layout is
// equivalent to a `double[3]` array. So the pointer return by
// `std::vector<Vector3D>::data` is compatible with the C type `(*double)[3] ==
// chfl_vector3d`.
static_assert(std::is_standard_layout<Vector3D>::value, "Vector3D must have a standard layout");

/// 3x3 matrix class
class Matrix3D final: private std::array<std::array<double, 3>, 3> {
    using super = std::array<std::array<double, 3>, 3>;
public:
    /// Create a Matrix3D by explicitly specifying all the components
    Matrix3D(double m11, double m12, double m13,
             double m21, double m22, double m23,
             double m31, double m32, double m33):
        super({{ {{m11, m12, m13}}, {{m21, m22, m23}}, {{m31, m32, m33}} }}) {}

    /// Create a diagonal Matrix3D with the three diagonal elements `a`, `b` and `c`
    Matrix3D(double a, double b, double c): Matrix3D(a, 0, 0, 0, b, 0, 0, 0, c) {}

    /// Create a Matrix3D with all components equal to 0
    Matrix3D(): Matrix3D(0, 0, 0) {}

    Matrix3D(const Matrix3D&) = default;
    Matrix3D& operator=(const Matrix3D&) = default;
    Matrix3D(Matrix3D&&) = default;
    Matrix3D& operator=(Matrix3D&&) = default;

    using super::operator[];

    /// Compute the inverse of this matrix
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
    return Vector3D(
        lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2],
        lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2],
        lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2]
    );
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

/// Compute the inverse of an invertible matrix.
inline Matrix3D Matrix3D::invert() const {
    double determinant = 0.0;
    determinant += (*this)[0][0] * ((*this)[1][1] * (*this)[2][2] - (*this)[2][1] * (*this)[1][2]);
    determinant -= (*this)[0][1] * ((*this)[1][0] * (*this)[2][2] - (*this)[1][2] * (*this)[2][0]);
    determinant += (*this)[0][2] * ((*this)[1][0] * (*this)[2][1] - (*this)[1][1] * (*this)[2][0]);;

    assert(determinant != 0.0 && "The matrix is not inversible!");
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
