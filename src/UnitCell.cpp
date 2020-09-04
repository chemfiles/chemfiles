// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cmath>
#include <cassert>
#include <array>

#include "chemfiles/UnitCell.hpp"
#include "chemfiles/types.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/unreachable.hpp"

using namespace chemfiles;

// Sinus and Cosine for degree values
constexpr double pi = 3.141592653589793238463;
inline double deg2rad(double x) {
    return x * pi / 180.0;
}

static double rad2deg(double x) {
    return x * 180.0 / pi;
}

static double cosd(double theta) {
    return cos(deg2rad(theta));
}

static double sind(double theta) {
    return sin(deg2rad(theta));
}

static bool is_roughly_zero(double value) {
    // We think that 0.00001 is close enough to 0
    return fabs(value) < 1e-5;
}

static bool is_roughly_90(double value) {
    // We think that 89.999° is close enough to 90°
    return fabs(value - 90.0) < 1e-3;
}

UnitCell::UnitCell(): UnitCell({0, 0, 0}) {}

UnitCell::UnitCell(Vector3D lengths): UnitCell(lengths, {90, 90, 90}) {}

UnitCell::UnitCell(Vector3D lengths, Vector3D angles):
    matrix_(Matrix3D::unit()),
    matrix_inv_(Matrix3D::unit()),
    lengths_(lengths),
    angles_(angles)
{
    
    if (is_roughly_zero(lengths_[0]) && is_roughly_zero(lengths_[1]) && is_roughly_zero(lengths_[2])) {
        shape_ = INFINITE;
        lengths_ = {0, 0, 0};
        angles_ = {90, 90, 90};
    } else if (is_roughly_90(angles_[0]) && is_roughly_90(angles_[1]) && is_roughly_90(angles_[2])) {
        shape_ = ORTHORHOMBIC;
        // Make sure alpha/beta/gamma are actually 90°, so that the matrix
        // update below does not create a non diagonal matrix.
        angles_ = {90, 90, 90};
    } else {
        shape_ = TRICLINIC;
    }
    update_matrix();
}

UnitCell::UnitCell(const Matrix3D& matrix): matrix_(Matrix3D::unit()), matrix_inv_(Matrix3D::unit()) {
    if (matrix[1][0] != 0 || matrix[2][0] != 0 || matrix[2][1] != 0) {
        throw error("the matrix supplied to UnitCell is not an upper triangular matrix");
    } else if (matrix_.determinant() <= 0) {
        throw error("the matrix supplied to UnitCell do not have a positive determinant");
    }

    if (is_roughly_zero(matrix[0][0]) && is_roughly_zero(matrix[1][1]) && is_roughly_zero(matrix[2][2]) &&
        is_roughly_zero(matrix[0][1]) && is_roughly_zero(matrix[0][2]) && is_roughly_zero(matrix[1][2])) {
        shape_ = INFINITE;
        lengths_ = {0, 0, 0};
        angles_ = {90, 90, 90};
    } else if (is_roughly_zero(matrix[0][1]) && is_roughly_zero(matrix[0][2]) && is_roughly_zero(matrix[1][2])) {
        shape_ = ORTHORHOMBIC;
        lengths_ = {matrix[0][0], matrix[1][1], matrix[2][2]};
        angles_ = {90, 90, 90};
    } else {
        shape_ = TRICLINIC;

        auto v1 = Vector3D(matrix[0][0], matrix[1][0], matrix[2][0]);
        auto v2 = Vector3D(matrix[0][1], matrix[1][1], matrix[2][1]);
        auto v3 = Vector3D(matrix[0][2], matrix[1][2], matrix[2][2]);
        
        auto angle = [](Vector3D& r1, Vector3D& r2) {
            return rad2deg(acos(dot(r1, r2) / (r1.norm() * r2.norm()))); 
        };

        lengths_ = {v1.norm(), v2.norm(), v3.norm()};
        angles_ = {angle(v2, v3), angle(v1, v3), angle(v1, v2)};
    }
    
    update_matrix();
    assert(fabs(matrix_[0][0] - matrix[0][0]) < 1e-5);
    assert(fabs(matrix_[1][0] - matrix[1][0]) < 1e-5);
    assert(fabs(matrix_[2][0] - matrix[2][0]) < 1e-5);

    assert(fabs(matrix_[0][1] - matrix[0][1]) < 1e-5);
    assert(fabs(matrix_[1][1] - matrix[1][1]) < 1e-5);
    assert(fabs(matrix_[2][1] - matrix[2][1]) < 1e-5);

    assert(fabs(matrix_[0][2] - matrix[0][2]) < 1e-5);
    assert(fabs(matrix_[1][2] - matrix[1][2]) < 1e-5);
    assert(fabs(matrix_[2][2] - matrix[2][2]) < 1e-5);
}

double UnitCell::volume() const {
    switch (shape_) {
    case INFINITE:
        return 0;
    case ORTHORHOMBIC:
        return lengths_[0] * lengths_[1] * lengths_[2];
    case TRICLINIC:
        return matrix_.determinant();
    }
}

void UnitCell::update_matrix() {
    matrix_[0][0] = lengths_[0];
    matrix_[1][0] = 0;
    matrix_[2][0] = 0;

    matrix_[0][1] = cosd(angles_[2]) * lengths_[1];
    matrix_[1][1] = sind(angles_[2]) * lengths_[1];
    matrix_[2][1] = 0;

    matrix_[0][2] = cosd(angles_[1]);
    matrix_[1][2] = (cosd(angles_[0]) - cosd(angles_[1]) * cosd(angles_[2])) / sind(angles_[2]);
    matrix_[2][2] = sqrt(1 - matrix_[0][2] * matrix_[0][2] - matrix_[1][2] * matrix_[1][2]);
    matrix_[0][2] *= lengths_[2];
    matrix_[1][2] *= lengths_[2];
    matrix_[2][2] *= lengths_[2];

    // Do not try to invert a cell with a 0 volume
    if (matrix_.determinant() <= 0.0) {
        matrix_inv_ = Matrix3D::unit();
    } else {
        matrix_inv_ = matrix_.invert();
    }
}

void UnitCell::set_shape(CellShape shape) {
    if (shape == ORTHORHOMBIC) {
        if (angles_ != Vector3D(90, 90, 90)) {
            throw error(
                "can not set cell shape to ORTHORHOMBIC: some angles are not 90°"
            );
        }
    } else if (shape == INFINITE) {
        if (angles_ != Vector3D(90, 90, 90)) {
            throw error(
                "can not set cell shape to INFINITE: some angles are not 90°"
            );
        }
        
        if (lengths_ != Vector3D(0, 0, 0)) {
            throw error(
                "can not set cell shape to INFINITE: some lengths are not 0"
            );
        }
    }

    shape_ = shape;
}

void UnitCell::set_lengths(Vector3D lengths) {
    if (shape_ == INFINITE) {
        throw error("can not set lengths for an infinite cell");
    }
    lengths_ = lengths;
    update_matrix();
}

void UnitCell::set_angles(Vector3D angles) {
    if (shape_ != TRICLINIC) {
        throw error("can not set angles for a non-triclinic cell");
    }
    angles_ = angles;
    update_matrix();
}

// Wrap a vector in an Orthorhombic UnitCell
Vector3D UnitCell::wrap_orthorhombic(const Vector3D& vector) const {
    return {
        vector[0] - round(vector[0] / lengths_[0]) * lengths_[0],
        vector[1] - round(vector[1] / lengths_[1]) * lengths_[1],
        vector[2] - round(vector[2] / lengths_[2]) * lengths_[2]
    };
}

// Wrap a vector in a triclinic UnitCell
Vector3D UnitCell::wrap_triclinic(const Vector3D& vector) const {
    auto fractional = matrix_inv_ * vector;
    fractional[0] -= round(fractional[0]);
    fractional[1] -= round(fractional[1]);
    fractional[2] -= round(fractional[2]);
    return matrix_ * fractional;
}

Vector3D UnitCell::wrap(const Vector3D& vector) const {
    switch (shape_) {
        case INFINITE:
            return vector;
        case ORTHORHOMBIC:
            return wrap_orthorhombic(vector);
        case TRICLINIC:
            return wrap_triclinic(vector);
    }
    unreachable();
}

namespace chemfiles {
    bool operator==(const UnitCell& rhs, const UnitCell& lhs) {
        if (lhs.shape() != rhs.shape()) {
            return false;
        }

        return rhs.lengths() == lhs.lengths() &&
               rhs.angles() == lhs.angles();
    }

    bool operator!=(const UnitCell& rhs, const UnitCell& lhs) {
        return !(rhs == lhs);
    }
}
