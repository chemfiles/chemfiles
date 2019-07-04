// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <array>
#include <algorithm>

#include "chemfiles/capi/types.h"
#include "chemfiles/capi/cell.h"
#include "chemfiles/capi/utils.hpp"
#include "chemfiles/capi/shared_allocator.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Frame.hpp"
using namespace chemfiles;

static_assert(sizeof(chfl_cellshape) == sizeof(int), "Wrong size for chfl_cellshape enum");

extern "C" CHFL_CELL* chfl_cell(const chfl_vector3d lengths) {
    CHFL_CELL* cell = nullptr;
    CHECK_POINTER_GOTO(lengths);
    CHFL_ERROR_GOTO(
        cell = shared_allocator::make_shared<UnitCell>(lengths[0], lengths[1], lengths[2]);
    )
    return cell;
error:
    chfl_free(cell);
    return nullptr;
}

extern "C" CHFL_CELL* chfl_cell_triclinic(const chfl_vector3d lengths, const chfl_vector3d angles) {
    CHFL_CELL* cell = nullptr;
    CHECK_POINTER_GOTO(lengths);
    CHECK_POINTER_GOTO(angles);
    CHFL_ERROR_GOTO(
        cell = shared_allocator::make_shared<UnitCell>(
            lengths[0], lengths[1], lengths[2],
            angles[0], angles[1], angles[2]
        );
        // ensure that the unit cell shape is always TRICLINIC, even if the
        // three angles are 90°.
        cell->set_shape(UnitCell::TRICLINIC);
    )
    return cell;
error:
    chfl_free(cell);
    return nullptr;
}


extern "C" CHFL_CELL* chfl_cell_from_frame(CHFL_FRAME* const frame) {
    CHFL_CELL* cell = nullptr;
    CHECK_POINTER_GOTO(frame);
    CHFL_ERROR_GOTO(
        cell = shared_allocator::shared_ptr<UnitCell>(frame, &frame->cell());
    )
    return cell;
error:
    chfl_free(cell);
    return nullptr;
}

extern "C" CHFL_CELL* chfl_cell_copy(const CHFL_CELL* const cell) {
    CHFL_CELL* new_cell = nullptr;
    CHFL_ERROR_GOTO(
        new_cell = shared_allocator::make_shared<UnitCell>(*cell);
    )
    return new_cell;
error:
    chfl_free(new_cell);
    return nullptr;
}

extern "C" chfl_status chfl_cell_volume(const CHFL_CELL* const cell, double* volume) {
    CHECK_POINTER(cell);
    CHECK_POINTER(volume);
    CHFL_ERROR_CATCH(
        *volume = cell->volume();
    )
}

extern "C" chfl_status chfl_cell_lengths(const CHFL_CELL* const cell, chfl_vector3d lengths) {
    CHECK_POINTER(cell);
    CHECK_POINTER(lengths);
    CHFL_ERROR_CATCH(
        lengths[0] = cell->a();
        lengths[1] = cell->b();
        lengths[2] = cell->c();
    )
}

extern "C" chfl_status chfl_cell_set_lengths(CHFL_CELL* const cell, const chfl_vector3d lengths) {
    CHECK_POINTER(cell);
    CHECK_POINTER(lengths);
    CHFL_ERROR_CATCH(
        cell->set_a(lengths[0]);
        cell->set_b(lengths[1]);
        cell->set_c(lengths[2]);
    )
}

extern "C" chfl_status chfl_cell_angles(const CHFL_CELL* const cell, chfl_vector3d angles) {
    CHECK_POINTER(cell);
    CHECK_POINTER(angles);
    CHFL_ERROR_CATCH(
        angles[0] = cell->alpha();
        angles[1] = cell->beta();
        angles[2] = cell->gamma();
    )
}

extern "C" chfl_status chfl_cell_set_angles(CHFL_CELL* const cell, const chfl_vector3d angles) {
    CHECK_POINTER(cell);
    CHFL_ERROR_CATCH(
        cell->set_alpha(angles[0]);
        cell->set_beta(angles[1]);
        cell->set_gamma(angles[2]);
    )
}

extern "C" chfl_status chfl_cell_matrix(const CHFL_CELL* const cell, chfl_vector3d matrix[3]) {
    CHECK_POINTER(cell);
    CHECK_POINTER(matrix);
    CHFL_ERROR_CATCH(
        auto cell_matrix = cell->matrix();
        std::copy(&cell_matrix[0][0], &cell_matrix[0][0] + 9, &matrix[0][0]);
    )
}

extern "C" chfl_status chfl_cell_shape(const CHFL_CELL* const cell, chfl_cellshape* const shape) {
    CHECK_POINTER(cell);
    CHECK_POINTER(shape);
    CHFL_ERROR_CATCH(
        *shape = static_cast<chfl_cellshape>(cell->shape());
    )
}

extern "C" chfl_status chfl_cell_set_shape(CHFL_CELL* const cell, chfl_cellshape shape) {
    CHECK_POINTER(cell);
    CHFL_ERROR_CATCH(
        cell->set_shape(static_cast<UnitCell::CellShape>(shape));
    )
}

extern "C" chfl_status chfl_cell_wrap(const CHFL_CELL* const cell, chfl_vector3d vector) {
    CHECK_POINTER(cell);
    CHECK_POINTER(vector);
    CHFL_ERROR_CATCH(
        auto result = cell->wrap(vector3d(vector));
        vector[0] = result[0];
        vector[1] = result[1];
        vector[2] = result[2];
    )
}
