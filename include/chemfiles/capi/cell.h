// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_CELL_H
#define CHEMFILES_CHFL_CELL_H

#include "chemfiles/capi/types.h"
#include "chemfiles/exports.h"

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

/// Create an unit cell using the optional `lengths` and `angles` parameters
///
/// If both `lengths` and `angles` are NULL, this creates an infinite unit cell.
/// If `angles` is NULL, it defaults to `[90, 90, 90]`.
///
/// The shape of the cell depends on the angles: it will be ORTHORHOMBIC if the
/// three angles are 90°, TRICLINIC otherwise.
///
/// The cell lengths should be in Angstroms, and the angles in degree.
///
/// The caller of this function should free the associated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_cell/chfl_cell.c}
/// @return A pointer to the unit cell, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_CELL* chfl_cell(const chfl_vector3d lengths, const chfl_vector3d angles);

/// Create an unit cell from the unit cell matrix.
///
/// If `matrix` contains only zeros, then an infinite cell is created. If only
/// the diagonal of the matrix is non-zero, then the cell is `ORTHORHOMBIC`.
/// Else a `TRICLINIC` cell is created. The matrix entries should be in
/// Angstroms.
///
/// The caller of this function should free the associated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_cell/from_matrix.c}
/// @return A pointer to the unit cell, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_CELL* chfl_cell_from_matrix(const chfl_vector3d matrix[3]);

/// Get access to the cell of a `frame`
///
/// Any modification to the cell will be reflected in the `frame`. The `frame`
/// will be kept alive, even if `chfl_free(frame)` is called, until `chfl_free`
/// is also called on the pointer returned by this function.
///
/// If `chfl_frame_set_cell` is called, this pointer will point to the new cell.
///
/// @example{capi/chfl_cell/from_frame.c}
/// @return A pointer to the unit cell, or NULL in case of error. You can use
///        `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_CELL* chfl_cell_from_frame(CHFL_FRAME* frame);

/// Get a copy of a `cell`.
///
/// The caller of this function should free the associated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_cell/copy.c}
/// @return A pointer to the new cell, or NULL in case of error. You can use
///         `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_CELL* chfl_cell_copy(const CHFL_CELL* cell);

/// Get the unit cell volume of `cell` in the double pointed to by `volume`.
///
/// @example{capi/chfl_cell/volume.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_volume(
    const CHFL_CELL* cell, double* volume
);

/// Get the unit cell lengths in `lengths`. The cell lengths are in Angstroms.
///
/// @example{capi/chfl_cell/lengths.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_lengths(
    const CHFL_CELL* cell, chfl_vector3d lengths
);

/// Set the unit cell lengths to `lengths`.
///
/// The cell lengths should be in Angstroms.
///
/// **This function reset cell orientation!**
///
/// After the call, the cell is aligned such that the first cell vector is along
/// the *x* axis, and the second cell vector is in the *xy* plane.
///
/// @example{capi/chfl_cell/set_lengths.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_set_lengths(
    CHFL_CELL* cell, const chfl_vector3d lengths
);

/// Get the cell angles in `angles`. The cell angles are in degrees.
///
/// @example{capi/chfl_cell/angles.c}
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
/// **This function reset cell orientation!**
///
/// After the call, the cell is aligned such that the first cell vector is along
/// the *x* axis, and the second cell vector is in the *xy* plane.
///
/// @example{capi/chfl_cell/set_angles.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_set_angles(
    CHFL_CELL* cell, const chfl_vector3d angles
);

/// Get the unit `cell` matricial representation in `matrix`.
///
/// @example{capi/chfl_cell/matrix.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_matrix(
    const CHFL_CELL* cell, chfl_vector3d matrix[3]
);

/// Get the unit `cell` shape in `shape`.
///
/// @example{capi/chfl_cell/shape.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_shape(
    const CHFL_CELL* cell, chfl_cellshape* shape
);

/// Set the unit `cell` shape to `shape`.
///
/// @example{capi/chfl_cell/set_shape.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_set_shape(
    CHFL_CELL* cell, chfl_cellshape shape
);

/// Wrap a `vector` in the unit `cell`.
///
/// @example{capi/chfl_cell/wrap.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_cell_wrap(
    const CHFL_CELL* cell, chfl_vector3d vector
);

#ifdef __cplusplus
}
#endif

#endif
