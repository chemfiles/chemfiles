// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/capi/frame.h"
#include "chemfiles/capi.hpp"

#include "chemfiles/Frame.hpp"
using namespace chemfiles;

extern "C" CHFL_FRAME* chfl_frame(uint64_t natoms) {
    CHFL_FRAME* frame = nullptr;
    CHFL_ERROR_GOTO(
        frame = new Frame(checked_cast(natoms));
    )
    return frame;
error:
    delete frame;
    return nullptr;
}

extern "C" CHFL_FRAME* chfl_frame_copy(const CHFL_FRAME* const frame) {
    CHFL_FRAME* new_frame = nullptr;
    CHFL_ERROR_GOTO(
        new_frame = new Frame(frame->clone());
    )
    return new_frame;
error:
    delete new_frame;
    return nullptr;
}

extern "C" chfl_status chfl_frame_atoms_count(const CHFL_FRAME* const frame, uint64_t *natoms) {
    CHECK_POINTER(frame);
    CHECK_POINTER(natoms);
    CHFL_ERROR_CATCH(
        *natoms = frame->natoms();
    )
}

extern "C" chfl_status chfl_frame_positions(CHFL_FRAME* const frame, chfl_vector_t** data, uint64_t* size) {
    CHECK_POINTER(frame);
    CHECK_POINTER(data);
    CHECK_POINTER(size);
    static_assert(
        sizeof(chfl_vector_t) == sizeof(Vector3D),
        "Wrong size for chfl_vector_t. It should match Vector3D."
    );
    CHFL_ERROR_CATCH(
        auto positions = frame->positions();
        *size = positions.size();
        *data = reinterpret_cast<chfl_vector_t*>(positions.data());
    )
}

extern "C" chfl_status chfl_frame_velocities(CHFL_FRAME* const frame, chfl_vector_t** data, uint64_t* size) {
    CHECK_POINTER(frame);
    CHECK_POINTER(data);
    CHECK_POINTER(size);
    static_assert(
        sizeof(chfl_vector_t) == sizeof(Vector3D),
        "Wrong size for chfl_vector_t. It should match Vector3D."
    );
    if (!frame->velocities()) {
        chemfiles::CAPI_LAST_ERROR = "No velocities in this frame!";
        return CHFL_MEMORY_ERROR;
    }
    CHFL_ERROR_CATCH(
        auto velocities = frame->velocities();
        *size = velocities->size();
        *data = reinterpret_cast<chfl_vector_t*>(velocities->data());
    )
}

extern "C" chfl_status chfl_frame_add_atom(
    CHFL_FRAME* const frame, const CHFL_ATOM* const atom,
    const chfl_vector_t position, const chfl_vector_t velocity) {
    CHECK_POINTER(frame);
    CHECK_POINTER(atom);
    CHECK_POINTER(position);
    CHFL_ERROR_CATCH(
        auto pos = vector3d(position[0], position[1], position[2]);
        if (velocity != nullptr) {
            auto vel = vector3d(velocity[0], velocity[1], velocity[2]);
            frame->add_atom(*atom, pos, vel);
        } else {
            frame->add_atom(*atom, pos);
        }
    )
}

extern "C" chfl_status chfl_frame_remove(CHFL_FRAME* const frame, uint64_t i) {
    CHECK_POINTER(frame);
    CHFL_ERROR_CATCH(
        frame->remove(static_cast<size_t>(i));
    )
}

extern "C" chfl_status chfl_frame_resize(CHFL_FRAME* const frame, uint64_t natoms) {
    CHECK_POINTER(frame);
    CHFL_ERROR_CATCH(
        frame->resize(checked_cast(natoms));
    )
}

extern "C" chfl_status chfl_frame_add_velocities(CHFL_FRAME* const frame) {
    CHECK_POINTER(frame);
    CHFL_ERROR_CATCH(
        frame->add_velocities();
    )
}

extern "C" chfl_status chfl_frame_has_velocities(const CHFL_FRAME* const frame, bool* has_velocities) {
    CHECK_POINTER(frame);
    CHECK_POINTER(has_velocities);
    CHFL_ERROR_CATCH(
        *has_velocities = bool(frame->velocities());
    )
}

extern "C" chfl_status chfl_frame_set_cell(CHFL_FRAME* const frame, const CHFL_CELL* const cell) {
    CHECK_POINTER(frame);
    CHECK_POINTER(cell);
    CHFL_ERROR_CATCH(
        frame->set_cell(*cell);
    )
}

extern "C" chfl_status chfl_frame_set_topology(CHFL_FRAME* const frame, const CHFL_TOPOLOGY* const topology) {
    CHECK_POINTER(frame);
    CHECK_POINTER(topology);
    CHFL_ERROR_CATCH(
        frame->set_topology(*topology);
    )
}

extern "C" chfl_status chfl_frame_step(const CHFL_FRAME* const frame, uint64_t* step) {
    CHECK_POINTER(frame);
    CHECK_POINTER(step);
    CHFL_ERROR_CATCH(
        *step = frame->step();
    )
}

extern "C" chfl_status chfl_frame_set_step(CHFL_FRAME* const frame, uint64_t step) {
    CHECK_POINTER(frame);
    CHFL_ERROR_CATCH(
        frame->set_step(checked_cast(step));
    )
}

extern "C" chfl_status chfl_frame_guess_topology(CHFL_FRAME* const frame) {
    CHECK_POINTER(frame);
    CHFL_ERROR_CATCH(
        frame->guess_topology();
    )
}

extern "C" chfl_status chfl_frame_free(CHFL_FRAME* const frame) {
    delete frame;
    return CHFL_SUCCESS;
}
