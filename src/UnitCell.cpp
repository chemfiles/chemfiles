// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cmath>

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/unreachable.hpp"
#include "chemfiles/UnitCell.hpp"
using namespace chemfiles;

// Sinus and Cosine for degree values
constexpr double pi = 3.141592653589793238463;
inline double deg2rad(double x) {
    return x * pi / 180.0;
}

inline double cosd(double theta) {
    return cos(deg2rad(theta));
}

inline double sind(double theta) {
    return sin(deg2rad(theta));
}

UnitCell::UnitCell(): UnitCell(INFINITE) {}

UnitCell::UnitCell(double a): UnitCell(a, a, a) {}

UnitCell::UnitCell(double a, double b, double c): UnitCell(ORTHORHOMBIC, a, b, c) {}

UnitCell::UnitCell(double a, double b, double c,
                   double alpha, double beta, double gamma)
: a_(a), b_(b), c_(c), alpha_(alpha), beta_(beta), gamma_(gamma){
    if (alpha_ == 90 && beta_ == 90 && gamma_ == 90) {
        shape_ = ORTHORHOMBIC;
    } else {
        shape_ = TRICLINIC;
    }
    update_matrix();
}

UnitCell::UnitCell(CellShape shape) : UnitCell(shape, 0) {}

UnitCell::UnitCell(CellShape shape, double a) : UnitCell(shape, a, a, a) {}

UnitCell::UnitCell(CellShape shape, double a, double b, double c)
    : a_(a), b_(b), c_(c), alpha_(90), beta_(90), gamma_(90), shape_(shape) {
    update_matrix();
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
        h_inv_ = Matrix3D();
    } else {
        h_inv_ = h_.invert();
    }
}

void UnitCell::raw_matricial(double matrix[3][3]) const {
    std::copy(&h_[0][0], &h_[0][0] + 9, &matrix[0][0]);
}

void UnitCell::shape(CellShape shape) {
    if (shape == ORTHORHOMBIC) {
        if (!(alpha_ == 90 && beta_ == 90 && gamma_ == 90)) {
            throw error(
                "can not be set shape to ORTHOROMBIC: some angles are not 90°"
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
Vector3D UnitCell::wrap_orthorombic(const Vector3D& vect) const {
    return Vector3D(
        vect[0] - round(vect[0] / a_) * a_,
        vect[1] - round(vect[1] / b_) * b_,
        vect[2] - round(vect[2] / c_) * c_
    );
}

// Wrap a vector in an Orthorombic UnitCell
Vector3D UnitCell::wrap_triclinic(const Vector3D& vect) const {
    auto fractional = h_inv_ * vect;
    fractional[0] -= round(fractional[0]);
    fractional[1] -= round(fractional[1]);
    fractional[2] -= round(fractional[2]);
    return h_ * fractional;
}

Vector3D UnitCell::wrap(const Vector3D& vect) const {
    switch (shape_) {
        case INFINITE:
            return vect;
        case ORTHORHOMBIC:
            return wrap_orthorombic(vect);
        case TRICLINIC:
            return wrap_triclinic(vect);
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
