/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
// clang-format off

#include "chemfiles.h"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

static_assert(sizeof(chfl_cell_shape_t) == sizeof(int), "Wrong size for chfl_cell_shape_t enum");

CHFL_CELL* chfl_cell(const chfl_vector_t lenghts) {
    assert(lenghts != nullptr);
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_GOTO(
        cell = new UnitCell(lenghts[0], lenghts[1], lenghts[2]);
    )
    return cell;
error:
    delete cell;
    return nullptr;
}

CHFL_CELL* chfl_cell_triclinic(const chfl_vector_t lenghts, const chfl_vector_t angles) {
    assert(lenghts != nullptr);
    assert(angles != nullptr);
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_GOTO(
        cell = new UnitCell(
            lenghts[0], lenghts[1], lenghts[2],
            angles[0], angles[1], angles[2]
        );
    )
    return cell;
error:
    delete cell;
    return nullptr;
}


CHFL_CELL* chfl_cell_from_frame(const CHFL_FRAME* const frame) {
    assert(frame != nullptr);
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_GOTO(
        cell = new UnitCell(frame->cell());
    )
    return cell;
error:
    delete cell;
    return nullptr;
}

chfl_status chfl_cell_volume(const CHFL_CELL* const cell, double* volume) {
    assert(cell != nullptr);
    assert(volume != nullptr);
    CHFL_ERROR_CATCH(
        *volume = cell->volume();
    )
}

chfl_status chfl_cell_lengths(const CHFL_CELL* const cell, chfl_vector_t lenghts) {
    assert(cell != nullptr);
    assert(lenghts != nullptr);
    CHFL_ERROR_CATCH(
        lenghts[0] = cell->a();
        lenghts[1] = cell->b();
        lenghts[2] = cell->c();
    )
}

chfl_status chfl_cell_set_lengths(CHFL_CELL* const cell, const chfl_vector_t lenghts) {
    assert(cell != nullptr);
    assert(lenghts != nullptr);
    CHFL_ERROR_CATCH(
        cell->set_a(lenghts[0]);
        cell->set_b(lenghts[1]);
        cell->set_c(lenghts[2]);
    )
}

chfl_status chfl_cell_angles(const CHFL_CELL* const cell, chfl_vector_t angles) {
    assert(cell != nullptr);
    assert(angles != nullptr);
    CHFL_ERROR_CATCH(
        angles[0] = cell->alpha();
        angles[1] = cell->beta();
        angles[2] = cell->gamma();
    )
}

chfl_status chfl_cell_set_angles(CHFL_CELL* const cell, const chfl_vector_t angles) {
    assert(cell != nullptr);
    CHFL_ERROR_CATCH(
        cell->set_alpha(angles[0]);
        cell->set_beta(angles[1]);
        cell->set_gamma(angles[2]);
    )
}

chfl_status chfl_cell_matrix(const CHFL_CELL* const cell, chfl_vector_t matrix[3]) {
    assert(cell != nullptr);
    assert(matrix != nullptr);
    CHFL_ERROR_CATCH(
        cell->raw_matricial(matrix);
    )
}

chfl_status chfl_cell_shape(const CHFL_CELL* const cell, chfl_cell_shape_t* const shape) {
    assert(cell != nullptr);
    assert(shape != nullptr);
    CHFL_ERROR_CATCH(
        *shape = static_cast<chfl_cell_shape_t>(cell->shape());
    )
}

chfl_status chfl_cell_set_shape(CHFL_CELL* const cell, chfl_cell_shape_t shape) {
    assert(cell != nullptr);
    CHFL_ERROR_CATCH(
        cell->shape(static_cast<UnitCell::CellShape>(shape));
    )
}

chfl_status chfl_cell_free(CHFL_CELL* cell) {
    delete cell;
    cell = nullptr;
    return CHFL_SUCCESS;
}
