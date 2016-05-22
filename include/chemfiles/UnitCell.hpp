/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_UNIT_CELL_HPP
#define CHEMFILES_UNIT_CELL_HPP

#ifdef WIN32
#undef INFINITE
#endif

#include "chemfiles/types.hpp"
#include "chemfiles/exports.hpp"

namespace chemfiles {

/*!
 * @class UnitCell UnitCell.hpp UnitCell.cpp
 * @brief An UnitCell represent the box containing the atoms, and its
 * periodicity
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
class CHFL_EXPORT UnitCell {
public:
    enum CellType {
        //! Orthorhombic cell, with the three angles equals to 90°
        ORTHORHOMBIC = 0,
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
    //! Construct a cubic unit cell of side size `a`
    UnitCell(double a);
    //! Construct an ORTHOROMBIC unit cell of side size `a`, `b`, `c`
    UnitCell(double a, double b, double c);
    //! Construct a TRICLINIC unit cell of side size `a`, `b`, `c`, and cell
    //! angles `alpha`, `beta`, `gamma`
    UnitCell(double a, double b, double c, double alpha, double beta, double gamma);
    //! Construct a cell of type `type`, with all lenghts set to 0 and all
    //! angles
    //! set to 90°
    UnitCell(CellType type);
    //! Construct a cell of type `type`, with all lenghts set to `a` and all
    //! angles
    //! set to 90°
    UnitCell(CellType type, double a);
    //! Construct a cell of type `type`, with lenghts set to `a` ,`b`, `d`,
    //! and all angles set to 90°
    UnitCell(CellType type, double a, double b, double c);

    ~UnitCell() = default;

    //! Get a matricial representation of the cell.
    Matrix3D matricial() const {
        return h_;
    }
    //! Populate C-style matricial representation of the cell. The array should
    //! have a 3 x 3 size.
    void raw_matricial(double[3][3]) const;

    //! Get the cell type
    CellType type() const { return type_; }
    //! Set the cell type to t
    void type(CellType t);

    //! Get the first lenght (a) of the cell
    double a() const { return a_; }
    //! Set the first lenght (a) of the cell
    void set_a(double val);
    //! Get the second lenght (b) of the cell
    double b() const { return b_; }
    //! Set the second lenght (b) of the cell
    void set_b(double val);
    //! Get the third lenght (c) of the cell
    double c() const { return c_; }
    //! Set the third lenght (c) of the cell
    void set_c(double val);

    //! Get the first angle (alpha) of the cell
    double alpha() const { return alpha_; }
    //! Set the first angle (alpha) of the cell if possible
    void set_alpha(double val);
    //! Get the second angle (beta) of the cell
    double beta() const { return beta_; }
    //! Set the second angle (beta) of the cell if possible
    void set_beta(double val);
    //! Get the third angle (gamma) of the cell
    double gamma() const { return gamma_; }
    //! Set the third angle (gamma) of the cell if possible
    void set_gamma(double val);

    //! Get the unit cell volume
    double volume() const;

    //! Wrap the vector `vect` in the unit cell, using periodic boundary
    //! conditions.
    Vector3D wrap(const Vector3D& vect) const;

private:
    /// Wrap a vector in orthorombic cell
    Vector3D wrap_orthorombic(const Vector3D& vect) const;
    /// Wrap a vector in triclinic cell
    Vector3D wrap_triclinic(const Vector3D& vect) const;
    /// Compute the cell matrix from the cell parameters
    void update_matrix();
    /// Caching the cell matrix
    Matrix3D h_;
    /// Caching the inverse of the cell matrix
    Matrix3D h_inv_;

    //! Cell lenghts
    double a_, b_, c_;
    //! Cell angles
    double alpha_, beta_, gamma_;
    //! Cell type
    CellType type_;
};

inline bool operator==(const UnitCell& rhs, const UnitCell& lhs) {
    if (lhs.type() != rhs.type())
        return false;

    return rhs.a() == lhs.a() &&
           rhs.b() == lhs.b() &&
           rhs.c() == lhs.c() &&
           rhs.alpha() == lhs.alpha() &&
           rhs.beta() == lhs.beta() &&
           rhs.gamma() == lhs.gamma();
}

} // namespace chemfiles

#endif
