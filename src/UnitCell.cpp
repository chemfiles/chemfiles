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

inline double rad2deg(double x) {
    return x * 180.0 / pi;
}

inline double cosd(double theta) {
    return cos(deg2rad(theta));
}

inline double sind(double theta) {
    return sin(deg2rad(theta));
}

UnitCell::UnitCell(): UnitCell(0) {
    shape_ = INFINITE;
}

UnitCell::UnitCell(double a): UnitCell(a, a, a) {}

UnitCell::UnitCell(double a, double b, double c): UnitCell(a, b, c, 90, 90, 90) {}

UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma):
    h_(Matrix3D::unit()),
    h_inv_(Matrix3D::unit()),
    a_(a),
    b_(b),
    c_(c),
    alpha_(alpha),
    beta_(beta),
    gamma_(gamma)
{
    auto is_90 = [](double angle) {
        // We think that 89.999° is close enough to 90°
        return fabs(angle - 90.0) < 1e-3;
    };
    if (is_90(alpha_) && is_90(beta_) && is_90(gamma_)) {
        shape_ = ORTHORHOMBIC;
        // Make sure alpha/beta/gamma are actually 90°, so that the matrix
        // update below does not create a non diagonal matrix.
        alpha_ = 90;
        beta_ = 90;
        gamma_ = 90;
    } else {
        shape_ = TRICLINIC;
    }
    update_matrix();
}

UnitCell::UnitCell(const Matrix3D& matrix): h_(Matrix3D::unit()), h_inv_(Matrix3D::unit()) {
    if (matrix[1][0] != 0 || matrix[2][0] != 0 || matrix[2][1] != 0) {
        throw error("matrix supplied to UnitCell is not an upper triangular matrix");
    }

    if (matrix[0][0] == 0 && matrix[1][1] == 0 && matrix[2][2] == 0 &&
        matrix[0][1] == 0 && matrix[0][2] == 0 && matrix[1][2] == 0) {

        shape_ = INFINITE;
        a_ = b_ = c_ = 0;
        alpha_ = beta_ = gamma_ = 90.0;

        return;
    }

    if (matrix[0][1] == 0 && matrix[0][2] == 0 && matrix[1][2] == 0) {
        shape_ = ORTHORHOMBIC;

        a_ = matrix[0][0];
        b_ = matrix[1][1];
        c_ = matrix[2][2];

        alpha_ = beta_ = gamma_ = 90.0;

        return;
    }

    shape_ = TRICLINIC;

    Vector3D v1 = {matrix[0][0], matrix[1][0], matrix[2][0]};
    Vector3D v2 = {matrix[0][1], matrix[1][1], matrix[2][1]};
    Vector3D v3 = {matrix[0][2], matrix[1][2], matrix[2][2]};

    a_ = v1.norm();
    b_ = v2.norm();
    c_ = v3.norm();

    alpha_ = rad2deg(acos(dot(v2, v3) / (b_ * c_)));
    beta_  = rad2deg(acos(dot(v1, v3) / (a_ * c_)));
    gamma_ = rad2deg(acos(dot(v1, v2) / (a_ * b_)));

    update_matrix();

    assert(fabs(h_[0][0] - matrix[0][0]) < 1e-5);
    assert(fabs(h_[1][0] - matrix[1][0]) < 1e-5);
    assert(fabs(h_[2][0] - matrix[2][0]) < 1e-5);

    assert(fabs(h_[0][1] - matrix[0][1]) < 1e-5);
    assert(fabs(h_[1][1] - matrix[1][1]) < 1e-5);
    assert(fabs(h_[2][1] - matrix[2][1]) < 1e-5);

    assert(fabs(h_[0][2] - matrix[0][2]) < 1e-5);
    assert(fabs(h_[1][2] - matrix[1][2]) < 1e-5);
    assert(fabs(h_[2][2] - matrix[2][2]) < 1e-5);
}

double UnitCell::volume() const {
    switch (shape_) {
    case INFINITE:
        return 0;
    case ORTHORHOMBIC:
        return a_ * b_ * c_;
    case TRICLINIC:
        break; // The computation is too complexe to take place in a switch
    }
    double cos_alpha = cos(deg2rad(alpha_));
    double cos_beta = cos(deg2rad(beta_));
    double cos_gamma = cos(deg2rad(gamma_));

    double factor = sqrt(
        1 - cos_alpha * cos_alpha - cos_beta * cos_beta -
        cos_gamma * cos_gamma + 2 * cos_alpha * cos_beta * cos_gamma
    );
    return a_ * b_ * c_ * factor;
}

void UnitCell::update_matrix() {
    h_[0][0] = a_;
    h_[1][0] = 0;
    h_[2][0] = 0;

    h_[0][1] = cosd(gamma_) * b_;
    h_[1][1] = sind(gamma_) * b_;
    h_[2][1] = 0;

    h_[0][2] = cosd(beta_);
    h_[1][2] = (cosd(alpha_) - cosd(beta_) * cosd(gamma_)) / sind(gamma_);
    h_[2][2] = sqrt(1 - h_[0][2] * h_[0][2] - h_[1][2] * h_[1][2]);
    h_[0][2] *= c_;
    h_[1][2] *= c_;
    h_[2][2] *= c_;

    // Do not try to invert a cell with a 0 volume
    if (volume() == 0.0) {
        h_inv_ = Matrix3D::unit();
    } else {
        h_inv_ = h_.invert();
    }
}

void UnitCell::set_shape(CellShape shape) {
    if (shape == ORTHORHOMBIC) {
        if (!(alpha_ == 90 && beta_ == 90 && gamma_ == 90)) {
            throw error(
                "can not be set cell shape to ORTHORHOMBIC: some angles are not 90°"
            );
        }
    } else if (shape == INFINITE) {
        if (!(alpha_ == 90 && beta_ == 90 && gamma_ == 90)) {
            throw error(
                "can not be set cell shape to INFINITE: some angles are not 90°"
            );
        }
        if (!(a_ == 0.0 && b_ == 0.0 && c_ == 0.0)) {
            throw error(
                "can not be set cell shape to INFINITE: some lengths are not 0"
            );
        }
    }

    shape_ = shape;
}

void UnitCell::set_a(double val) {
    if (shape_ == INFINITE) {
        throw error("can not set 'a' on infinite cell");
    }
    a_ = val;
    update_matrix();
}

void UnitCell::set_b(double val) {
    if (shape_ == INFINITE) {
        throw error("can not set 'b' on infinite cell");
    }
    b_ = val;
    update_matrix();
}

void UnitCell::set_c(double val) {
    if (shape_ == INFINITE) {
        throw error("can not set 'c' on infinite cell");
    }
    c_ = val;
    update_matrix();
}

void UnitCell::set_alpha(double val) {
    if (shape_ != TRICLINIC) {
        throw error("can not set 'alpha' on non triclinic cell");
    }
    alpha_ = val;
    update_matrix();
}

void UnitCell::set_beta(double val) {
    if (shape_ != TRICLINIC) {
        throw error("can not set 'beta' on non triclinic cell");
    }
    beta_ = val;
    update_matrix();
}

void UnitCell::set_gamma(double val) {
    if (shape_ != TRICLINIC) {
        throw error("can not set 'gamma' on non triclinic cell");
    }
    gamma_ = val;
    update_matrix();
}

// Wrap a vector in an Orthorombic UnitCell
Vector3D UnitCell::wrap_orthorombic(const Vector3D& vector) const {
    return {
        vector[0] - round(vector[0] / a_) * a_,
        vector[1] - round(vector[1] / b_) * b_,
        vector[2] - round(vector[2] / c_) * c_
    };
}

// Wrap a vector in an Orthorombic UnitCell
Vector3D UnitCell::wrap_triclinic(const Vector3D& vector) const {
    auto fractional = h_inv_ * vector;
    fractional[0] -= round(fractional[0]);
    fractional[1] -= round(fractional[1]);
    fractional[2] -= round(fractional[2]);
    return h_ * fractional;
}

Vector3D UnitCell::wrap(const Vector3D& vector) const {
    switch (shape_) {
        case INFINITE:
            return vector;
        case ORTHORHOMBIC:
            return wrap_orthorombic(vector);
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

        return rhs.a() == lhs.a() &&
               rhs.b() == lhs.b() &&
               rhs.c() == lhs.c() &&
               rhs.alpha() == lhs.alpha() &&
               rhs.beta() == lhs.beta() &&
               rhs.gamma() == lhs.gamma();
    }

    bool operator!=(const UnitCell& rhs, const UnitCell& lhs) {
        return !(rhs == lhs);
    }
}
