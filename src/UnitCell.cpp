// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cmath>
#include <cassert>
#include <array>

#include "chemfiles/UnitCell.hpp"
#include "chemfiles/types.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/unreachable.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

// Sinus and Cosine for degree values
constexpr double pi = 3.141592653589793238463;
static double deg2rad(double x) {
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

namespace chemfiles { namespace details {
    bool is_roughly_zero(double value) {
        // We think that 0.00001 is close enough to 0
        return fabs(value) < 1e-5;
    }

    bool is_roughly_90(double value) {
        // We think that 89.999° is close enough to 90°
        return fabs(value - 90.0) < 1e-3;
    }

    bool is_diagonal(const Matrix3D& matrix) {
        return is_roughly_zero(matrix[1][0]) && is_roughly_zero(matrix[2][0]) &&
               is_roughly_zero(matrix[0][1]) && is_roughly_zero(matrix[2][1]) &&
               is_roughly_zero(matrix[0][2]) && is_roughly_zero(matrix[1][2]);
    }

    bool is_lower_triangular(const Matrix3D& matrix) {
        return is_roughly_zero(matrix[0][1]) && is_roughly_zero(matrix[0][2]) &&
               is_roughly_zero(matrix[1][2]);
    }
}}

using namespace chemfiles::details;

static void check_lengths(const Vector3D& lengths) {
    if (lengths[0] < 0 || lengths[1] < 0 || lengths[2] < 0) {
        throw error("a unit cell can not have negative lengths");
    }

    if (lengths != Vector3D(0, 0, 0) && (is_roughly_zero(lengths[0]) || is_roughly_zero(lengths[1]) || is_roughly_zero(lengths[2]))) {
        warning("", "trying to set one or two unit cell lengths to zero, something might be wrong");
    }
}

static void check_angles(const Vector3D& angles) {
    if (angles[0] < 0 || angles[1] < 0 || angles[2] < 0) {
        throw error("a unit cell can not have negative angles");
    }

    if (is_roughly_zero(angles[0]) || is_roughly_zero(angles[1]) || is_roughly_zero(angles[2])) {
        throw error("a unit cell can not have 0° angles");
    }

    if (angles[0] >= 180 || angles[1] >= 180 || angles[2] >= 180) {
        throw error("a unit cell can not have angles larger than or equal to 180°");
    }
}

static Matrix3D cell_matrix_from_lenths_angles(Vector3D lengths, Vector3D angles) {
    check_lengths(lengths);
    check_angles(angles);

    if (is_roughly_90(angles[0]) && is_roughly_90(angles[1]) && is_roughly_90(angles[2])) {
        angles = {90, 90, 90};
    }

    auto matrix = Matrix3D::zero();

    matrix[0][0] = lengths[0];
    matrix[0][1] = 0;
    matrix[0][2] = 0;

    matrix[1][0] = cosd(angles[2]) * lengths[1];
    matrix[1][1] = sind(angles[2]) * lengths[1];
    matrix[1][2] = 0;

    matrix[2][0] = cosd(angles[1]);
    matrix[2][1] = (cosd(angles[0]) - cosd(angles[1]) * cosd(angles[2])) / sind(angles[2]);
    matrix[2][2] = sqrt(1 - matrix[2][0] * matrix[2][0] - matrix[2][1] * matrix[2][1]);
    matrix[2][0] *= lengths[2];
    matrix[2][1] *= lengths[2];
    matrix[2][2] *= lengths[2];

    assert(is_lower_triangular(matrix));

    return matrix;
}

static Vector3D calc_lengths_from_cell_matrix(const Matrix3D& matrix) {
    auto v1 = Vector3D(matrix[0]);
    auto v2 = Vector3D(matrix[1]);
    auto v3 = Vector3D(matrix[2]);

    return {v1.norm(), v2.norm(), v3.norm()};
}

static Vector3D calc_angles_from_cell_matrix(const Matrix3D& matrix) {
    auto v1 = Vector3D(matrix[0]);
    auto v2 = Vector3D(matrix[1]);
    auto v3 = Vector3D(matrix[2]);

    return {
        rad2deg(acos(dot(v2, v3) / (v2.norm() * v3.norm()))),
        rad2deg(acos(dot(v1, v3) / (v1.norm() * v3.norm()))),
        rad2deg(acos(dot(v1, v2) / (v1.norm() * v2.norm())))
    };
}

static bool is_infinite(const Vector3D& lengths) {
    return is_roughly_zero(lengths[0])
        && is_roughly_zero(lengths[1])
        && is_roughly_zero(lengths[2]);
}

static bool is_orthorhombic(const Vector3D& lengths, const Vector3D& angles) {
    if (is_infinite(lengths)) {
        return false;
    }
    // we support cells with one or two lengths of 0 which results in NaN angles
    return (is_roughly_90(angles[0]) || std::isnan(angles[0]))
        && (is_roughly_90(angles[1]) || std::isnan(angles[1]))
        && (is_roughly_90(angles[2]) || std::isnan(angles[2]));
}

UnitCell::UnitCell(): UnitCell({0, 0, 0}) {}

UnitCell::UnitCell(Vector3D lengths): UnitCell(lengths, {90, 90, 90}) {}

UnitCell::UnitCell(Vector3D lengths, Vector3D angles):
    UnitCell(cell_matrix_from_lenths_angles(lengths, angles)) {}

UnitCell::UnitCell(Matrix3D matrix): matrix_(matrix), matrix_inv_transposed_(Matrix3D::unit()) {
    auto determinant = matrix_.determinant();
    if (determinant < 0.0) {
        throw error("invalid unit cell matrix with negative determinant");
    }

    auto lengths = calc_lengths_from_cell_matrix(matrix_);
    auto angles = calc_angles_from_cell_matrix(matrix_);
    if (!is_diagonal(matrix_) && is_orthorhombic(lengths, angles)) {
        throw error("orthorhombic cell must have their a vector along x axis, b vector along y axis and c vector along z axis");
    }

    if (is_diagonal(matrix_)) {
        if (is_roughly_zero(matrix_[0][0]) && is_roughly_zero(matrix_[1][1]) && is_roughly_zero(matrix_[2][2])) {
            shape_ = INFINITE;
            matrix_ = Matrix3D::zero();
        } else {
            shape_ = ORTHORHOMBIC;
        }
    } else {
        shape_ = TRICLINIC;
    }

    if (!is_roughly_zero(this->volume())) {
        // Do not try to invert a cell with a 0 volume
        matrix_inv_transposed_ = matrix_.invert().transpose();
    }
}

double UnitCell::volume() const {
    switch (shape_) {
    case INFINITE:
        return 0;
    case ORTHORHOMBIC:
    case TRICLINIC:
        return matrix_.determinant();
    default:
        unreachable();
    }
}

void UnitCell::set_shape(CellShape shape) {
    if (shape == ORTHORHOMBIC) {
        if (!is_diagonal(matrix_)) {
            throw error(
                "can not set cell shape to ORTHORHOMBIC: some angles are not 90°"
            );
        }
    } else if (shape == INFINITE) {
        if (!is_diagonal(matrix_)) {
            throw error(
                "can not set cell shape to INFINITE: some angles are not 90°"
            );
        }

        auto lengths = this->lengths();
        if (!(is_roughly_zero(lengths[0]) && is_roughly_zero(lengths[1]) && is_roughly_zero(lengths[2]))) {
            throw error(
                "can not set cell shape to INFINITE: some lengths are not 0"
            );
        }
    }

    shape_ = shape;
}

Vector3D UnitCell::lengths() const {
    switch (shape_) {
    case INFINITE:
        return {0, 0, 0};
    case ORTHORHOMBIC:
        return {matrix_[0][0], matrix_[1][1], matrix_[2][2]};
    case TRICLINIC:
        return calc_lengths_from_cell_matrix(matrix_);
    default:
        unreachable();
    }
}

Vector3D UnitCell::angles() const {
    switch (shape_) {
    case INFINITE:
    case ORTHORHOMBIC:
        return {90, 90, 90};
    case TRICLINIC:
        return calc_angles_from_cell_matrix(matrix_);
    default:
        unreachable();
    }
}

void UnitCell::set_lengths(Vector3D lengths) {
    if (shape_ == INFINITE) {
        throw error("can not set lengths for an infinite cell");
    }

    check_lengths(lengths);

    if (!is_lower_triangular(matrix_)) {
        warning("UnitCell", "resetting unit cell orientation in set_lengths");
    }

    // Reset the unit cell, and remove any existing rotation.
    *this = UnitCell(lengths, this->angles());
}

void UnitCell::set_angles(Vector3D angles) {
    if (shape_ != TRICLINIC) {
        throw error("can not set angles for a non-triclinic cell");
    }

    check_angles(angles);

    if (!is_lower_triangular(matrix_)) {
        warning("UnitCell", "resetting unit cell orientation in set_angles");
    }

    // Reset the unit cell, and remove any existing rotation.
    *this = UnitCell(this->lengths(), angles);
}

Vector3D UnitCell::wrap_orthorhombic(const Vector3D& vector) const {
    auto lengths = this->lengths();
    return {
        vector[0] - round(vector[0] / lengths[0]) * lengths[0],
        vector[1] - round(vector[1] / lengths[1]) * lengths[1],
        vector[2] - round(vector[2] / lengths[2]) * lengths[2]
    };
}

Vector3D UnitCell::wrap_triclinic(const Vector3D& vector) const {
    auto fractional = matrix_inv_transposed_ * vector;

    // Rounding each fractional component independently is only a minimum
    // image operation for orthogonal cells. In a skewed lattice, a nearby
    // image can require changing two or more lattice coordinates together.
    // Start with the component-wise image and use it to establish a finite
    // sphere around the target in fractional space. The Frobenius norm of the
    // inverse cell is an upper bound on the inverse operator norm, so every
    // image that can improve the starting candidate lies in this sphere.
    auto rounded = fractional;
    rounded[0] -= round(rounded[0]);
    rounded[1] -= round(rounded[1]);
    rounded[2] -= round(rounded[2]);
    auto best = matrix_.transpose() * rounded;
    auto best_squared = dot(best, best);

    auto inverse = matrix_inv_transposed_.transpose();
    auto inverse_frobenius_squared = 0.0;
    for (const auto& row: inverse) {
        for (auto value: row) {
            inverse_frobenius_squared += value * value;
        }
    }
    auto radius = sqrt(best_squared * inverse_frobenius_squared);
    auto radius_squared = radius * radius;

    auto lower = [radius](double value) {
        return static_cast<long long>(floor(value - radius));
    };
    auto upper = [radius](double value) {
        return static_cast<long long>(ceil(value + radius));
    };
    auto i0 = lower(fractional[0]);
    auto i1 = lower(fractional[1]);
    auto i2 = lower(fractional[2]);
    auto j0 = upper(fractional[0]);
    auto j1 = upper(fractional[1]);
    auto j2 = upper(fractional[2]);

    for (auto a = i0; a <= j0; a++) {
        for (auto b = i1; b <= j1; b++) {
            for (auto c = i2; c <= j2; c++) {
                auto candidate_fractional = Vector3D(
                    fractional[0] - static_cast<double>(a),
                    fractional[1] - static_cast<double>(b),
                    fractional[2] - static_cast<double>(c)
                );
                auto candidate = matrix_.transpose() * candidate_fractional;
                auto candidate_squared = dot(candidate, candidate);
                if (candidate_squared < best_squared && candidate_squared <= radius_squared) {
                    best = candidate;
                    best_squared = candidate_squared;
                }
            }
        }
    }
    return best;
}

Vector3D UnitCell::wrap(const Vector3D& vector) const {
    switch (shape_) {
    case INFINITE:
        return vector;
    case ORTHORHOMBIC:
        return wrap_orthorhombic(vector);
    case TRICLINIC:
        return wrap_triclinic(vector);
    default:
        unreachable();
    }
}

namespace chemfiles {
    bool operator==(const UnitCell& rhs, const UnitCell& lhs) {
        if (lhs.shape() != rhs.shape()) {
            return false;
        }

        return rhs.matrix() == lhs.matrix();
    }

    bool operator!=(const UnitCell& rhs, const UnitCell& lhs) {
        return !(rhs == lhs);
    }
}
