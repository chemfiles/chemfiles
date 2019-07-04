// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UNIT_CELL_HPP
#define CHEMFILES_UNIT_CELL_HPP

#include "chemfiles/types.hpp"
#include "chemfiles/exports.h"

#ifdef CHEMFILES_WINDOWS
#undef INFINITE
#endif

namespace chemfiles {

/// An UnitCell represent the box containing the atoms, and its periodicity
///
/// A unit cell is fully represented by three lengths (a, b, c); and three angles
/// (alpha, beta, gamma). The angles are stored in degrees, and the lengths in
/// Angstroms.
///
/// A cell also has a matricial representation, by projecting the three base
/// vector into an orthonormal base. We choose to represent such matrix as an
/// upper triangular matrix:
///
/// ```
/// | a_x   b_x   c_x |
/// |  0    b_y   c_y |
/// |  0     0    c_z |
/// ```
class CHFL_EXPORT UnitCell final {
public:
    /// Possible shapes for the unit cell
    enum CellShape {
        /// Orthorhombic cell, with the three angles equals to 90°
        ORTHORHOMBIC = 0,
        /// Triclinic cell, with any values for the angles.
        TRICLINIC = 1,
        /// Infinite cell, to use when there is no cell
        INFINITE = 2
    };

    ~UnitCell() = default;
    UnitCell(const UnitCell& other) = default;
    UnitCell& operator=(const UnitCell& other) = default;
    UnitCell(UnitCell&& other) = default;
    UnitCell& operator=(UnitCell&& other) = default;

    /// Construct an `INFINITE` unit cell, with all lengths set to 0
    ///
    /// @example{cell/cell-0.cpp}
    UnitCell();

    /// Construct a cubic unit cell of side size `a`
    ///
    /// @example{cell/cell-1.cpp}
    UnitCell(double a);

    /// Construct an `ORTHOROMBIC` unit cell of side size `a`, `b`, `c`
    ///
    /// @example{cell/cell-3.cpp}
    UnitCell(double a, double b, double c);

    /// Construct a unit cell of side size `a`, `b`, `c`, and cell angles
    /// `alpha`, `beta`, `gamma`.
    ///
    /// If all of `alpha`, `beta` and `gamma` are 90.0, then the cell is
    /// `ORTHOROMBIC`. Else a `TRICLINIC` cell is created.
    ///
    /// @example{cell/cell-6.cpp}
    UnitCell(double a, double b, double c, double alpha, double beta, double gamma);

    /// Construct a unit cell via from an upper triangular matrix.
    ///
    /// If a matrix of all zeros is given, then an infinite cell is 
    /// created.
    ///
    /// If only the diagonal of the matrix is non-zero, then the cell is
    /// `ORTHOROMBIC`. Else a `TRICLINIC` cell is created.
    ///
    /// @example{cell/matrix.cpp}
    UnitCell(const Matrix3D& matrix);

    /// Get the cell matrix, defined as the upper triangular matrix
    ///
    /// ```
    /// | a_x   b_x   c_x |
    /// |  0    b_y   c_y |
    /// |  0     0    c_z |
    /// ```
    ///
    /// @example{cell/matrix.cpp}
    Matrix3D matrix() const {
        return h_;
    }

    /// Get the cell shape
    ///
    /// @example{cell/shape.cpp}
    CellShape shape() const { return shape_; }

    /// Set the cell shape to `shape`
    ///
    /// @example{cell/shape.cpp}
    ///
    /// @throws Error if `shape` is `ORTHORHOMBIC` and some angles are not 90°,
    ///         or if `shape` is `INFINITE` and some lengths are not 0.0.
    void set_shape(CellShape shape);

    /// Get the first lenght (a) of the cell
    ///
    /// @example{cell/lengths.cpp}
    double a() const { return a_; }

    /// Set the first lenght (a) of the cell
    ///
    /// @example{cell/lengths.cpp}
    ///
    /// @throws Error if the cell shape is `INFINITE`.
    void set_a(double val);

    /// Get the second lenght (b) of the cell
    ///
    /// @example{cell/lengths.cpp}
    double b() const { return b_; }

    /// Set the second lenght (b) of the cell
    ///
    /// @example{cell/lengths.cpp}
    ///
    /// @throws Error if the cell shape is `INFINITE`.
    void set_b(double val);

    /// Get the third lenght (c) of the cell
    ///
    /// @example{cell/lengths.cpp}
    double c() const { return c_; }

    /// Set the third lenght (c) of the cell
    ///
    /// @example{cell/lengths.cpp}
    ///
    /// @throws Error if the cell shape is `INFINITE`.
    void set_c(double val);

    /// Get the first angle (alpha) of the cell
    ///
    /// @example{cell/angles.cpp}
    double alpha() const { return alpha_; }

    /// Set the first angle (alpha) of the cell
    ///
    /// @example{cell/angles.cpp}
    ///
    /// @throws Error if the cell shape is not `TRICLINIC`.
    void set_alpha(double val);

    /// Get the second angle (beta) of the cell
    ///
    /// @example{cell/angles.cpp}
    double beta() const { return beta_; }

    /// Set the second angle (beta) of the cell if possible
    ///
    /// @example{cell/angles.cpp}
    ///
    /// @throws Error if the cell shape is not `TRICLINIC`.
    void set_beta(double val);

    /// Get the third angle (gamma) of the cell
    ///
    /// @example{cell/angles.cpp}
    double gamma() const { return gamma_; }

    /// Set the third angle (gamma) of the cell if possible
    ///
    /// @example{cell/angles.cpp}
    ///
    /// @throws Error if the cell shape is not `TRICLINIC`.
    void set_gamma(double val);

    /// Get the unit cell volume
    ///
    /// @example{cell/volume.cpp}
    double volume() const;

    /// Wrap the `vector` in the unit cell, using periodic boundary conditions.
    ///
    /// For an orthorombic unit cell, this make sure that all the vector
    /// components are between `-L/2` and `L/2` where `L` is the corresponding
    /// cell length.
    ///
    /// @example{cell/wrap.cpp}
    Vector3D wrap(const Vector3D& vector) const;

private:
    /// Wrap a vector in orthorombic cell
    Vector3D wrap_orthorombic(const Vector3D& vector) const;
    /// Wrap a vector in triclinic cell
    Vector3D wrap_triclinic(const Vector3D& vector) const;
    /// Compute the cell matrix from the cell parameters
    void update_matrix();
    /// Caching the cell matrix
    Matrix3D h_;
    /// Caching the inverse of the cell matrix
    Matrix3D h_inv_;

    /// Cell lengths
    double a_, b_, c_;
    /// Cell angles
    double alpha_, beta_, gamma_;
    /// Cell type
    CellShape shape_;
};

/// Exact comparison of unit cells.
///
/// This performs an exact comparison of the two unit cells, using floating
/// point equality. This means that the two cells have to be exactly identical,
/// not only very close.
CHFL_EXPORT bool operator==(const UnitCell& rhs, const UnitCell& lhs);
CHFL_EXPORT bool operator!=(const UnitCell& rhs, const UnitCell& lhs);

} // namespace chemfiles

#endif
