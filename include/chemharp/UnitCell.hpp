/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_UNIT_CELL_HPP
#define HARP_UNIT_CELL_HPP

#ifdef WIN32
    #undef INFINITE
#endif

#include <array>
#include "chemharp/exports.hpp"

namespace harp {

class Vector3D;

//! 3 x 3 matrix type
typedef std::array<std::array<double, 3>, 3> Matrix3D;

/*!
 * @class UnitCell UnitCell.hpp UnitCell.cpp
 * @brief An UnitCell represent the box containing the atoms, and its periodicity
 *
 * A unit cell is fully represented by three lenghts (a, b, c); and three angles
 * (alpha, beta, gamma). The angles are stored in degrees, and the lenghts in
 * Angstroms.
 *
 * A cell also has a matricial representation, by projecting the three base
 * vector into an orthonormal base. We choose to represent such matrix as an
 * upper triangular matrix:
 *
 * 				| a_x   b_x   c_x |
 * 				|  0    b_y   c_y |
 * 				|  0     0    c_z |
 *
 * An unit cell also have a cell type, represented by the `CellType` enum.
 */
class CHRP_EXPORT UnitCell {
public:
    enum CellType {
        //! Orthorombic cell, with the three angles equals to 90°
        ORTHOROMBIC = 0,
        //! Triclinic cell, with any values for the angles.
        TRICLINIC = 1,
        //! Infinite cell, to use when there is no cell
        INFINITE = 2
    };

    //! Copy constructor
    UnitCell(const UnitCell& other) = default;
    UnitCell& operator=(const UnitCell& other) = default;
    //! Move constructor
    UnitCell(UnitCell&& other) = default;
    UnitCell& operator=(UnitCell&& other) = default;

    //! Construct an INFINITE unit cell
    UnitCell();
    //! Construct a cubic unit cell of side size \c a
    UnitCell(double a);
    //! Construct an ORTHOROMBIC unit cell of side size \c a, \c b, \c c
    UnitCell(double a, double b, double c);
    //! Construct a TRICLINIC unit cell of side size \c a, \c b, \c c, and cell
    //! angles \c alpha, \c beta, \c gamma
    UnitCell(double a, double b, double c, double alpha, double beta, double gamma);
    //! Construct a cell of type \c type, with all lenghts set to 0 and all angles
    //! set to 90°
    UnitCell(CellType type);
    //! Construct a cell of type \c type, with all lenghts set to \c a and all angles
    //! set to 90°
    UnitCell(CellType type, double a);
    //! Construct a cell of type \c type, with lenghts set to \c a ,\c b, \c d,
    //! and all angles set to 90°
    UnitCell(CellType type, double a, double b, double c);

    ~UnitCell() = default;

    //! Get a matricial representation of the cell.
    Matrix3D matricial() const;
    //! Populate C-style matricial representation of the cell. The array should
    //! have a 3 x 3 size.
    void raw_matricial(double[3][3]) const;

    //! Get the cell type
    CellType type() const {return _type;}
    //! Set the cell type to t
    void type(CellType t);

    //! Get the first lenght (a) of the cell
    double a() const {return _a;}
    //! Set the first lenght (a) of the cell
    void a(double val);
    //! Get the second lenght (b) of the cell
    double b() const {return _b;}
    //! Set the second lenght (b) of the cell
    void b(double val);
    //! Get the third lenght (c) of the cell
    double c() const {return _c;}
    //! Set the third lenght (c) of the cell
    void c(double val);

    //! Get the first angle (alpha) of the cell
    double alpha() const {return _alpha;}
    //! Set the first angle (alpha) of the cell if possible
    void alpha(double val);
    //! Get the second angle (beta) of the cell
    double beta() const {return _beta;}
    //! Set the second angle (beta) of the cell if possible
    void beta(double val);
    //! Get the third angle (gamma) of the cell
    double gamma() const {return _gamma;}
    //! Set the third angle (gamma) of the cell if possible
    void gamma(double val);

    //! Get the unit cell volume
    double volume() const;

    //! Get the cell periodicity for the x axis
    bool periodic_x() const {return pbc_x;}
    //! Get the cell periodicity for the y axis
    bool periodic_y() const {return pbc_y;}
    //! Get the cell periodicity for the z axis
    bool periodic_z() const {return pbc_z;}
    //! True if the cell is periodic in the three dimmensions
    bool full_periodic() const {return pbc_x && pbc_y && pbc_z;}

    //! Set the cell periodicity for the x axis
    void periodic_x(bool p) {pbc_x = p;}
    //! Set the cell periodicity for the y axis
    void periodic_y(bool p) {pbc_y = p;}
    //! Set the cell periodicity for the z axis
    void periodic_z(bool p) {pbc_z = p;}
    //! Set the cell periodicity in three dimmensions
    void full_periodic(bool p) {pbc_x = p; pbc_y = p; pbc_z = p;}

    //! Wrap the vector \c vect in the unit cell
    Vector3D wrap(const Vector3D& vect) const;
private:
    //! Cell lenghts
    double _a, _b, _c;
    //! Cell angles
    double _alpha, _beta, _gamma;
    //! Cell type
    CellType _type;
    //! Cell periodicity
    bool pbc_x, pbc_y, pbc_z;
};

inline bool operator==(const UnitCell& rhs, const UnitCell& lhs) {
    if (lhs.type() != rhs.type())
        return false;

    return rhs.a() == lhs.a() && rhs.b() == lhs.b() && rhs.c() == lhs.c() &&
           rhs.alpha() == lhs.alpha() && rhs.beta() == lhs.beta() && rhs.gamma() == lhs.gamma() &&
           rhs.periodic_x() == lhs.periodic_x() && rhs.periodic_y() == lhs.periodic_y() && rhs.periodic_z() == lhs.periodic_z();
}

} // namespace harp

#endif
