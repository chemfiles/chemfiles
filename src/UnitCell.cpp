/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include <cassert>
#include <cmath>

#include "UnitCell.hpp"
#include "Error.hpp"
using namespace harp;

// Sinus and Cosine for degree values
constexpr double pi = 3.1415926535897;
inline double cosd(double theta) {return cos(theta*pi/180);}
inline double sind(double theta) {return sin(theta*pi/180);}

UnitCell::UnitCell() : UnitCell(INFINITE) {}

UnitCell::UnitCell(double a) : UnitCell(a, a, a) {}

UnitCell::UnitCell(double a, double b, double c) : UnitCell(a, b, c, 90, 90, 90) {}

UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma)
: _a(a), _b(b), _c(c), _alpha(alpha), _beta(beta), _gamma(gamma), pbc_x(false),
pbc_y(false), pbc_z(false) {
    if (alpha == 90 && beta == 90 && gamma == 90)
        _type = ORTHOROMBIC;
    else
        _type = TRICLINIC;
}

UnitCell::UnitCell(CellType type) : UnitCell(type, 0) {}

UnitCell::UnitCell(CellType type, double a) : UnitCell(type, a, a, a) {}

UnitCell::UnitCell(CellType type, double a, double b, double c)
: _a(a), _b(b), _c(c), _alpha(90), _beta(90), _gamma(90), _type(type), pbc_x(false),
pbc_y(false), pbc_z(false) {}



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
    assert(_type != INFINITE);
    _a = val;
}


void UnitCell::b(double val){
    assert(_type != INFINITE);
    _b = val;
}


void UnitCell::c(double val){
    assert(_type != INFINITE);
    _c = val;
}

void UnitCell::alpha(double val){
    assert(_type == TRICLINIC);
    _alpha = val;
}

void UnitCell::beta(double val){
    assert(_type == TRICLINIC);
    _beta = val;
}

void UnitCell::gamma(double val){
    assert(_type == TRICLINIC);
    _gamma = val;
}
