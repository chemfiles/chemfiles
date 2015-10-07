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
#include "chemfiles/Vector3D.hpp"
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
: _a(a), _b(b), _c(c), _alpha(alpha), _beta(beta), _gamma(gamma), pbc_x(true),
pbc_y(true), pbc_z(true) {
    if (alpha == 90 && beta == 90 && gamma == 90)
        _type = ORTHOROMBIC;
    else
        _type = TRICLINIC;
}

UnitCell::UnitCell(CellType type) : UnitCell(type, 0) {}

UnitCell::UnitCell(CellType type, double a) : UnitCell(type, a, a, a) {}

UnitCell::UnitCell(CellType type, double a, double b, double c)
: _a(a), _b(b), _c(c), _alpha(90), _beta(90), _gamma(90), _type(type), pbc_x(true),
pbc_y(true), pbc_z(true) {}

double UnitCell::volume() const {
    switch (_type) {
        case INFINITE:
            return 0;
        case ORTHOROMBIC:
            return _a * _b * _c;
        case TRICLINIC:
            break; // The computation is too complexe to take place in a switch
    }
    double cos_alpha = cos(deg2rad(_alpha));
    double cos_beta = cos(deg2rad(_beta));
    double cos_gamma = cos(deg2rad(_gamma));

    double factor = sqrt(1 - cos_alpha*cos_alpha - cos_beta*cos_beta - cos_gamma*cos_gamma
                         + 2 * cos_alpha * cos_beta * cos_gamma);
    return _a * _b * _c * factor;
}

Matrix3D UnitCell::matricial() const {
    auto mat = Matrix3D();
    mat[0][0] = _a;

    mat[1][0] = cosd(_gamma) * _b;
    mat[1][1] = sind(_gamma) * _b;

    mat[2][0] = cosd(_beta);
    mat[2][1] = (cosd(_alpha) - cosd(_beta)*cosd(_gamma)) / sind(_gamma);
    mat[2][2] = sqrt(1 - mat[2][0]*mat[2][0] - mat[2][1]*mat[2][1]);
    mat[2][0] *= _c;
    mat[2][1] *= _c;
    mat[2][2] *= _c;

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
        if (!(_alpha == 90 && _beta == 90 && _gamma == 90)){
            throw Error("UnitCell type can not be set to ORTHOROMBIC : some"
                            " angles are not 90°");
        }
    }
    _type = type;
}

void UnitCell::a(double val){
    if (_type == INFINITE)
        throw Error("Can not set 'a' on infinite cell");
    _a = val;
}


void UnitCell::b(double val){
    if (_type == INFINITE)
        throw Error("Can not set 'b' on infinite cell");
    _b = val;
}


void UnitCell::c(double val){
    if (_type == INFINITE)
        throw Error("Can not set 'c' on infinite cell");
    _c = val;
}

void UnitCell::alpha(double val){
    if (_type != TRICLINIC)
        throw Error("Can not set 'alpha' on non triclinic cell");
    _alpha = val;
}

void UnitCell::beta(double val){
    if (_type != TRICLINIC)
        throw Error("Can not set 'beta' on non triclinic cell");
    _beta = val;
}

void UnitCell::gamma(double val){
    if (_type != TRICLINIC)
        throw Error("Can not set 'gamma' on non triclinic cell");
    _gamma = val;
}

// Wrap a vector in an Orthorombic UnitCell
static Vector3D wrap_orthorombic(const UnitCell& cell, const Vector3D& vect) {
    Vector3D res;
    res[0] = static_cast<float>(vect[0] - round(vect[0]/cell.a())*cell.a());
    res[1] = static_cast<float>(vect[1] - round(vect[1]/cell.b())*cell.b());
    res[2] = static_cast<float>(vect[2] - round(vect[2]/cell.c())*cell.c());
    return res;
}

// Wrap a vector in an Orthorombic UnitCell
static Vector3D wrap_triclinic(const UnitCell& cell, const Vector3D& vect) {
    Vector3D res = vect;

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

Vector3D UnitCell::wrap(const Vector3D& vect) const{
    if (!full_periodic()){
        // TODO: implement this
        throw Error("Unimplemend vector wrapping for non fully-periodic cells.");
    }

    if (_type == INFINITE)
        return vect;
    else if (_type == ORTHOROMBIC)
        return wrap_orthorombic(*this, vect);
    else if (_type == TRICLINIC)
        return wrap_triclinic(*this, vect);
    else
        throw Error("Unknown cell type when wrapping a vector.");
}
