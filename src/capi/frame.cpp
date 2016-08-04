/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
// clang-format off

#include "chemfiles.h"
#include "chemfiles/Frame.hpp"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

CHFL_FRAME* chfl_frame(size_t natoms) {
    CHFL_FRAME* frame = nullptr;
    CHFL_ERROR_GOTO(
        frame = new Frame(natoms);
    )
    return frame;
error:
    delete frame;
    return nullptr;
}

int chfl_frame_atoms_count(const CHFL_FRAME* frame, size_t *natoms) {
    assert(frame != nullptr);
    assert(natoms != nullptr);
    CHFL_ERROR_CATCH(
        *natoms = frame->natoms();
    )
}

int chfl_frame_positions(CHFL_FRAME* frame, float (**data)[3], size_t* size) {
    assert(frame != nullptr);
    assert(data != nullptr);
    assert(size != nullptr);
    CHFL_ERROR_CATCH(
        auto positions = frame->positions();
        *size = positions.size();
        *data = reinterpret_cast<float(*)[3]>(positions.data());
    )
}

int chfl_frame_velocities(CHFL_FRAME* frame, float (**data)[3], size_t* size) {
    assert(frame != nullptr);
    assert(data != nullptr);
    assert(size != nullptr);
    if (!frame->velocities()) {
        chemfiles::CAPI_LAST_ERROR = "No velocities in this frame!";
        return CHFL_MEMORY_ERROR;
    }
    CHFL_ERROR_CATCH(
        auto velocities = frame->velocities();
        *size = velocities->size();
        *data = reinterpret_cast<float(*)[3]>(velocities->data());
    )
}

int chfl_frame_resize(CHFL_FRAME* frame, size_t natoms) {
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        frame->resize(natoms);
    )
}

int chfl_frame_add_velocities(CHFL_FRAME* frame) {
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        frame->add_velocities();
    )
}

int chfl_frame_has_velocities(const CHFL_FRAME* frame, bool* has_velocities) {
    assert(frame != nullptr);
    assert(has_velocities != nullptr);
    CHFL_ERROR_CATCH(
        *has_velocities = bool(frame->velocities());
    )
}

int chfl_frame_set_cell(CHFL_FRAME* frame, const CHFL_CELL* cell) {
    assert(frame != nullptr);
    assert(cell != nullptr);
    CHFL_ERROR_CATCH(
        frame->set_cell(*cell);
    )
}

int chfl_frame_set_topology(CHFL_FRAME* frame, const CHFL_TOPOLOGY* topology) {
    assert(frame != nullptr);
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        frame->set_topology(*topology);
    )
}

int chfl_frame_step(const CHFL_FRAME* frame, size_t* step) {
    assert(frame != nullptr);
    assert(step != nullptr);
    CHFL_ERROR_CATCH(
        *step = frame->step();
    )
}

int chfl_frame_set_step(CHFL_FRAME* frame, size_t step) {
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        frame->set_step(step);
    )
}

int chfl_frame_guess_topology(CHFL_FRAME* frame) {
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        frame->guess_topology();
    )
}

int chfl_frame_free(CHFL_FRAME* frame) {
    delete frame;
    frame = nullptr;
    return CHFL_SUCCESS;
}
