// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_CELL_H
#define CHEMFILES_CHFL_CELL_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// Available cell shapes in chemfiles
typedef enum {  // NOLINT: this is both a C and C++ file
    /// The three angles are 90°
    CHFL_CELL_ORTHORHOMBIC = 0,
    /// The three angles may not be 90°
    CHFL_CELL_TRICLINIC = 1,
    /// Cell shape when there is no periodic boundary conditions
    CHFL_CELL_INFINITE = 2,
} chfl_cellshape;

/// Create an unit cell from three `lengths`. The unit cell shape is
/// `CHFL_CELL_ORTHORHOMBIC`.
///
/// The cell lengths should be in Angstroms.
///
/// The caller of this function should free the associated memory using
/// `chfl_cell_free`.
///
/// @example{tests/capi/doc/chfl_cell/chfl_cell.c}
/// @return A pointer to the unit cell, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_CELL* chfl_cell(const chfl_vector3d lengths);

/// Create an unit cell from three `lengths` and three `angles`. The unit cell
/// shape is `CHFL_CELL_TRICLINIC`.
///
/// The cell lengths should be in Angstroms, and the angles in degree.
///
/// The cell angles are defined as follow: alpha is the angles between the cell
/// vector `b` and `c`; beta as the angle between `a` and `c`; and gamma as the
/// angle between `a` and `b`.
///
/// The caller of this function should free the associated memory using
/// `chfl_cell_free`.
///
/// @example{tests/capi/doc/chfl_cell/triclinic.c}
/// @return A pointer to the unit cell, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_CELL* chfl_cell_triclinic(
    const chfl_vector3d lengths, const chfl_vector3d angles
);

/// Get a copy of the unit cell of a `frame`.
///
/// The caller of this function should free the associated memory using
/// `chfl_cell_free`.
///
/// @example{tests/capi/doc/chfl_cell/from_frame.c}
/// @return A pointer to the unit cell, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_CELL* chfl_cell_from_frame(const CHFL_FRAME* frame);

/// Get a copy of a `cell`.
///
/// The caller of this function should free the associated memory using
/// `chfl_cell_free`.
///
/// @example{tests/capi/doc/chfl_cell/copy.c}
/// @return A pointer to the new cell, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_CELL* chfl_cell_copy(const CHFL_CELL* cell);

/// Get the unit cell volume of `cell` in the double pointed to by `volume`.
///
/// @example{tests/capi/doc/chfl_cell/volume.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_volume(
    const CHFL_CELL* cell, double* volume
);

/// Get the unit cell lengths in `lengths`. The cell lengths are in Angstroms.
///
/// @example{tests/capi/doc/chfl_cell/lengths.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_lengths(
    const CHFL_CELL* cell, chfl_vector3d lengths
);

/// Set the unit cell lengths to `lengths`.
///
/// The cell lengths should be in Angstroms.
///
/// @example{tests/capi/doc/chfl_cell/set_lengths.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_set_lengths(
    CHFL_CELL* cell, const chfl_vector3d lengths
);

/// Get the cell angles in `angles`. The cell angles are in degrees.
///
/// @example{tests/capi/doc/chfl_cell/angles.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_angles(
    const CHFL_CELL* cell, chfl_vector3d angles
);

/// Set the cell angles to `angles`.
///
/// The cell lengths should be in degree. Trying to set cell angles on a cell
/// which is not triclinic (does not have the `CHFL_CELL_TRICLINIC` shape) is
/// an error.
///
/// @example{tests/capi/doc/chfl_cell/set_angles.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_set_angles(
    CHFL_CELL* cell, const chfl_vector3d angles
);

/// Get the unit `cell` matricial representation in `matrix`.
///
/// The unit cell representation is obtained by aligning the a vector along the
/// *x* axis and putting the b vector in the *xy* plane. This make the matrix
/// an upper triangular matrix:
/// ```
/// | a_x   b_x   c_x |
/// |  0    b_y   c_y |
/// |  0     0    c_z |
/// ```
///
/// @example{tests/capi/doc/chfl_cell/matrix.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_matrix(
    const CHFL_CELL* cell, chfl_vector3d matrix[3]
);

/// Get the unit `cell` shape in `shape`.
///
/// @example{tests/capi/doc/chfl_cell/shape.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_shape(
    const CHFL_CELL* cell, chfl_cellshape* shape
);

/// Set the unit `cell` shape to `shape`.
///
/// @example{tests/capi/doc/chfl_cell/set_shape.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_set_shape(
    CHFL_CELL* cell, chfl_cellshape shape
);

/// Wrap a `vector` in the unit `cell`.
///
/// @example{tests/capi/doc/chfl_cell/wrap.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_wrap(
    const CHFL_CELL* cell, chfl_vector3d vector
);

/// Free the memory associated with a `cell`.
///
/// @example{tests/capi/doc/chfl_cell/chfl_cell.c}
/// @return `CHFL_SUCCESS`
CHFL_EXPORT chfl_status chfl_cell_free(CHFL_CELL* cell);

#ifdef __cplusplus
}
#endif

#endif
