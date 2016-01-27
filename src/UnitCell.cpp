/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include <cassert>
#include <cmath>

#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

// Sinus and Cosine for degree values
constexpr double pi = 3.141592653589793238463;
inline double deg2rad(double x) {return x * pi / 180.0;}
inline double cosd(double theta) {return cos(deg2rad(theta));}
inline double sind(double theta) {return sin(deg2rad(theta));}

UnitCell::UnitCell() : UnitCell(INFINITE) {}

UnitCell::UnitCell(double a) : UnitCell(a, a, a) {}

UnitCell::UnitCell(double a, double b, double c) : UnitCell(a, b, c, 90, 90, 90) {}

UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma)
: a_(a), b_(b), c_(c), alpha_(alpha), beta_(beta), gamma_(gamma) {
    if (alpha == 90 && beta == 90 && gamma == 90)
        type_ = ORTHOROMBIC;
    else
        type_ = TRICLINIC;
}

UnitCell::UnitCell(CellType type) : UnitCell(type, 0) {}

UnitCell::UnitCell(CellType type, double a) : UnitCell(type, a, a, a) {}

UnitCell::UnitCell(CellType type, double a, double b, double c)
: a_(a), b_(b), c_(c), alpha_(90), beta_(90), gamma_(90), type_(type) {}

double UnitCell::volume() const {
    switch (type_) {
        case INFINITE:
            return 0;
        case ORTHOROMBIC:
            return a_ * b_ * c_;
        case TRICLINIC:
            break; // The computation is too complexe to take place in a switch
    }
    double cos_alpha = cos(deg2rad(alpha_));
    double cos_beta = cos(deg2rad(beta_));
    double cos_gamma = cos(deg2rad(gamma_));

    double factor = sqrt(1 - cos_alpha*cos_alpha - cos_beta*cos_beta - cos_gamma*cos_gamma
                         + 2 * cos_alpha * cos_beta * cos_gamma);
    return a_ * b_ * c_ * factor;
}

Matrix3D UnitCell::matricial() const {
    auto mat = Matrix3D();
    mat[0][0] = a_;

    mat[1][0] = cosd(gamma_) * b_;
    mat[1][1] = sind(gamma_) * b_;

    mat[2][0] = cosd(beta_);
    mat[2][1] = (cosd(alpha_) - cosd(beta_)*cosd(gamma_)) / sind(gamma_);
    mat[2][2] = sqrt(1 - mat[2][0]*mat[2][0] - mat[2][1]*mat[2][1]);
    mat[2][0] *= c_;
    mat[2][1] *= c_;
    mat[2][2] *= c_;

    return mat;
}

void UnitCell::raw_matricial(double mat[3][3]) const {
    auto cpp_mat = matricial();
    for (size_t i=0; i<3; i++){
        for (size_t j=0; j<3; j++){
            mat[i][j] = cpp_mat[i][j];
        }
    }
}


void UnitCell::type(CellType type){
    if (type == ORTHOROMBIC) {
        if (!(alpha_ == 90 && beta_ == 90 && gamma_ == 90)){
            throw Error("UnitCell type can not be set to ORTHOROMBIC : some"
                            " angles are not 90°");
        }
    }
    type_ = type;
}

void UnitCell::set_a(double val){
    if (type_ == INFINITE)
        throw Error("Can not set 'a' on infinite cell");
    a_ = val;
}


void UnitCell::set_b(double val){
    if (type_ == INFINITE)
        throw Error("Can not set 'b' on infinite cell");
    b_ = val;
}


void UnitCell::set_c(double val){
    if (type_ == INFINITE)
        throw Error("Can not set 'c' on infinite cell");
    c_ = val;
}

void UnitCell::set_alpha(double val){
    if (type_ != TRICLINIC)
        throw Error("Can not set 'alpha' on non triclinic cell");
    alpha_ = val;
}

void UnitCell::set_beta(double val){
    if (type_ != TRICLINIC)
        throw Error("Can not set 'beta' on non triclinic cell");
    beta_ = val;
}

void UnitCell::set_gamma(double val){
    if (type_ != TRICLINIC)
        throw Error("Can not set 'gamma' on non triclinic cell");
    gamma_ = val;
}

// Wrap a vector in an Orthorombic UnitCell
static std::array<float, 3> wrap_orthorombic(const UnitCell& cell, const std::array<float, 3>& vect) {
    std::array<float, 3> res;
    res[0] = static_cast<float>(vect[0] - round(vect[0]/cell.a())*cell.a());
    res[1] = static_cast<float>(vect[1] - round(vect[1]/cell.b())*cell.b());
    res[2] = static_cast<float>(vect[2] - round(vect[2]/cell.c())*cell.c());
    return res;
}

// Wrap a vector in an Orthorombic UnitCell
static std::array<float, 3> wrap_triclinic(const UnitCell& cell, const std::array<float, 3>& vect) {
    std::array<float, 3> res = vect;

    auto mat = cell.matricial();
    for (size_t i=2 ; i != static_cast<size_t>(-1) ; i--) {
        while (fabs(res[i]) > mat[i][i]/2) {
            if (res[i] < 0) {
                res[0] += static_cast<float>(mat[i][0]);
                res[1] += static_cast<float>(mat[i][1]);
                res[2] += static_cast<float>(mat[i][2]);
            } else {
                res[0] -= static_cast<float>(mat[i][0]);
                res[1] -= static_cast<float>(mat[i][1]);
                res[2] -= static_cast<float>(mat[i][2]);
            }
        }
    }
    return res;
}

std::array<float, 3> UnitCell::wrap(const std::array<float, 3>& vect) const{
    if (type_ == INFINITE)
        return vect;
    else if (type_ == ORTHOROMBIC)
        return wrap_orthorombic(*this, vect);
    else if (type_ == TRICLINIC)
        return wrap_triclinic(*this, vect);
    else
        throw Error("Unknown cell type when wrapping a vector.");
}
