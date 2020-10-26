// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UNIT_CELL_HPP
#define CHEMFILES_UNIT_CELL_HPP

#include "chemfiles/types.hpp"
#include "chemfiles/exports.h"
#include "chemfiles/config.h"  // IWYU pragma: keep

#ifdef CHEMFILES_WINDOWS
#undef INFINITE
#endif

namespace chemfiles {

namespace private_details {
    /// check if a single value is close enough to zero to be considered equal
    /// to zero, in the context of unit cell matrices
    bool is_roughly_zero(double value);

    /// check if a single value is close enough to 90 to be considered equal
    /// to 90, in the context of unit cell matrices
    bool is_roughly_90(double value);

    /// check if a matrix is diagonal according to `is_roughly_zero`
    bool is_diagonal(const Matrix3D& matrix);

    /// check if a matrix is an upper triangular matrix according to
    /// `is_roughly_zero`
    bool is_upper_triangular(const Matrix3D& matrix);
}

/// An UnitCell represent the box containing the atoms, and its periodicity
///
/// A unit cell is represented by the cell matrix, containing the three cell
/// vectors:
/// ```
/// | a_x   b_x   c_x |
/// | a_y   b_y   c_y |
/// | a_z   b_z   c_z |
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
    /// If all lengths are set to 0, then the cell is `INFINITE`. If at least
    /// one length is not zero and all angles are 90.0, then the cell is
    /// `ORTHORHOMBIC`.  Else a `TRICLINIC` cell is created.
    ///
    /// @example{cell/cell-2.cpp}
    UnitCell(Vector3D lengths, Vector3D angles);

    /// Construct a unit cell from a cell matrix.
    ///
    /// If `matrix` contains only zeros, then an infinite cell is created. If
    /// only the diagonal of the matrix is non-zero, then the cell is
    /// `ORTHORHOMBIC`.  Else a `TRICLINIC` cell is created. The matrix entries
    /// should be in Angstroms.
    ///
    /// @example{cell/cell-matrix.cpp}
    UnitCell(Matrix3D matrix);

    /// Get the cell matrix
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

    /// Get the lengths of the cell's vectors, in angstroms.
    ///
    /// @example{cell/lengths.cpp}
    Vector3D lengths() const;

    /// Get the angles between the cell's vectors in degrees
    ///
    /// @example{cell/angles.cpp}
    Vector3D angles() const;

    /// Set the lengths of the cell's vectors. The values should be in angstroms.
    ///
    /// **This function reset cell orientation!**
    ///
    /// After the call, the cell is aligned such that the first cell vector is
    /// along the *x* axis, and the second cell vector is in the *xy* plane.
    ///
    /// @example{cell/lengths.cpp}
    /// @throws Error if the cell shape is `INFINITE`.
    void set_lengths(Vector3D lengths);

    /// Set the angles between the cell's vectors. The values should be in degrees.
    ///
    /// **This function reset cell orientation!**
    ///
    /// After the call, the cell is aligned such that the first cell vector is
    /// along the *x* axis, and the second cell vector is in the *xy* plane.
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

    /// Cell matrix
    Matrix3D matrix_;
    /// Caching the inverse of the cell matrix
    Matrix3D matrix_inv_;
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
