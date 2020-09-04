// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UNIT_CELL_HPP
#define CHEMFILES_UNIT_CELL_HPP

#include "chemfiles/types.hpp"
#include "chemfiles/exports.h"
#include "chemfiles/config.h"

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
    UnitCell(UnitCell&& other) noexcept = default;
    UnitCell& operator=(UnitCell&& other) noexcept = default;

    /// Construct an `INFINITE` unit cell, with all lengths set to 0
    ///
    /// @example{cell/cell-0.cpp}
    UnitCell();

    /// Construct an `ORTHORHOMBIC` unit cell with the given cell `lengths`
    ///
    /// @example{cell/cell-1.cpp}
    UnitCell(Vector3D lengths);

    /// Construct a unit cell with the given cell `lengths` and `angles`
    ///
    /// If all lengths are set to 0, then the cell is `INFINITE`
    /// If all lengths are not zero and all angles are 90.0, then the cell is `ORTHORHOMBIC`.
    /// Else a `TRICLINIC` cell is created.
    ///
    /// @example{cell/from-2.cpp}
    UnitCell(Vector3D lengths, Vector3D angles);

    /// Construct a unit cell via from an upper triangular matrix.
    ///
    /// If a matrix of all zeros is given, then an infinite cell is
    /// created.
    ///
    /// If only the diagonal of the matrix is non-zero, then the cell is
    /// `ORTHORHOMBIC`. Else a `TRICLINIC` cell is created.
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
        return matrix_;
    }

    /// Get the cell shape
    ///
    /// @example{cell/shape.cpp}
    CellShape shape() const {
        return shape_;
    }

    /// Set the cell shape to `shape`
    ///
    /// @example{cell/shape.cpp}
    /// @throws Error if `shape` is `ORTHORHOMBIC` and some angles are not 90°,
    ///         or if `shape` is `INFINITE` and some lengths are not 0.0.
    void set_shape(CellShape shape);

    /// Get the lenghts of the cell's vectors, in angstroms.
    ///
    /// @example{cell/lengths.cpp}
    Vector3D lengths() const {
        return lengths_;
    }
    
    /// Get the angles between the cell's vectors in degrees
    ///
    /// @example{cell/angles.cpp}
    Vector3D angles() const {
        return angles_;
    }

    /// Set the lenghts of the cell's vectors. The values should be in angstroms.
    ///
    /// @example{cell/lengths.cpp}
    /// @throws Error if the cell shape is `INFINITE`.
    void set_lengths(Vector3D lengths);

    /// Set the angles between the cell's vectors. The values should be in degrees.
    ///
    /// @example{cell/angles.cpp}
    /// @throws Error if the cell shape is not `TRICLINIC`.
    void set_angles(Vector3D angles);

    /// Get the unit cell volume
    ///
    /// @example{cell/volume.cpp}
    double volume() const;

    /// Wrap the `vector` in the unit cell, using periodic boundary conditions.
    ///
    /// For an orthorhombic unit cell, this make sure that all the vector
    /// components are between `-L/2` and `L/2` where `L` is the corresponding
    /// cell length.
    ///
    /// @example{cell/wrap.cpp}
    Vector3D wrap(const Vector3D& vector) const;

private:
    /// Wrap a vector in orthorhombic cell
    Vector3D wrap_orthorhombic(const Vector3D& vector) const;
    /// Wrap a vector in triclinic cell
    Vector3D wrap_triclinic(const Vector3D& vector) const;
    /// Compute the cell matrix from the cell parameters
    void update_matrix();
    /// Caching the cell matrix
    Matrix3D matrix_;
    /// Caching the inverse of the cell matrix
    Matrix3D matrix_inv_;

    /// Cell lengths
    Vector3D lengths_;
    /// Cell angles
    Vector3D angles_;
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
