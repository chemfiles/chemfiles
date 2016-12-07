// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_CELL_H
#define CHEMFILES_CHFL_CELL_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// Available cell shapes in chemfiles
typedef enum CHFL_CELL_SHAPE {
    /// The three angles are 90°
    CHFL_CELL_ORTHORHOMBIC = 0,
    /// The three angles may not be 90°
    CHFL_CELL_TRICLINIC = 1,
    /// Cell shape when there is no periodic boundary conditions
    CHFL_CELL_INFINITE = 2,
} chfl_cell_shape_t;

/// @brief Create an ORTHORHOMBIC unit cell from the three lenghts
/// @param lenghts the three lenght of the cell (in Angstroms)
/// @return A pointer to the unit cell, or NULL in case of error
CHFL_EXPORT CHFL_CELL* chfl_cell(const chfl_vector_t lenghts);

/// @brief Create a TRICLINIC unit cell from the three lenghts and the three angles
/// @param lenghts the three lenght of the cell (in Angstroms)
/// @param angles the three angles of the cell (in degree)
/// @return A pointer to the unit cell, or NULL in case of error
///
/// The cell angles are defined like this: alpha is the angles between the cell
/// vector `b` and `c`; beta as the angle between `a` and `c`; and gamma as the
/// angle between `a` and `b`.
CHFL_EXPORT CHFL_CELL* chfl_cell_triclinic(
    const chfl_vector_t lenghts, const chfl_vector_t angles
);

/// @brief Get the unit cell from a frame
/// @param frame the frame
/// @return A pointer to the unit cell, or NULL in case of error
CHFL_EXPORT CHFL_CELL* chfl_cell_from_frame(const CHFL_FRAME* const frame);

/// @brief Get the cell volume.
/// @param cell the unit cell to read
/// @param volume the volume
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_volume(
    const CHFL_CELL* const cell, double* volume
);

/// @brief Get the cell lenghts.
/// @param cell the unit cell to read
/// @param lenghts the three lenght of the cell (in Angstroms)
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_lengths(
    const CHFL_CELL* const cell, chfl_vector_t lenghts
);

/// @brief Set the unit cell lenghts.
/// @param cell the unit cell to modify
/// @param lenghts the three lenght of the cell (in Angstroms)
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_set_lengths(
    CHFL_CELL* const cell, const chfl_vector_t lenghts
);

/// @brief Get the cell angles, in degrees.
/// @param cell the cell to read
/// @param angles the three angles of the cell (in degree)
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_angles(
    const CHFL_CELL* const cell, chfl_vector_t angles
);

/// @brief Set the cell angles, in degrees. This is only possible for TRICLINIC
/// cells.
/// @param cell the unit cell to modify
/// @param angles the three angles of the cell (in degree)
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_set_angles(
    CHFL_CELL* const cell, const chfl_vector_t angles
);

/// @brief Get the unit cell matricial representation.
/// @param cell the unit cell to use
/// @param matrix the matrix to fill. It should be a 3x3 matrix.
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_matrix(
    const CHFL_CELL* const cell, chfl_vector_t matrix[3]
);

/// @brief Get the cell shape
/// @param cell the unit cell to read
/// @param shape the shape of the cell
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_shape(
    const CHFL_CELL* const cell, chfl_cell_shape_t* const shape
);

/// @brief Set the cell shape
/// @param cell the cell to modify
/// @param shape the new shape of the cell
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_set_shape(
    CHFL_CELL* const cell, chfl_cell_shape_t shape
);

/// @brief Destroy an unit cell, and free the associated memory
/// @param cell The cell to destroy
/// @return The status code
CHFL_EXPORT chfl_status chfl_cell_free(CHFL_CELL* const cell);

#ifdef __cplusplus
}
#endif

#endif
