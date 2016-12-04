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

extern "C" chfl_status chfl_frame_atoms_count(const CHFL_FRAME* const frame, uint64_t *natoms) {
    assert(frame != nullptr);
    assert(natoms != nullptr);
    CHFL_ERROR_CATCH(
        *natoms = frame->natoms();
    )
}

extern "C" chfl_status chfl_frame_positions(CHFL_FRAME* const frame, chfl_vector_t** data, uint64_t* size) {
    assert(frame != nullptr);
    assert(data != nullptr);
    assert(size != nullptr);
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
    assert(frame != nullptr);
    assert(data != nullptr);
    assert(size != nullptr);
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

extern "C" chfl_status chfl_frame_add_atom(CHFL_FRAME* const frame, const CHFL_ATOM* const atom, chfl_vector_t position, chfl_vector_t velocity) {
    assert(frame != nullptr);
    assert(atom != nullptr);
    assert(position != nullptr);
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

extern "C" chfl_status chfl_frame_resize(CHFL_FRAME* const frame, uint64_t natoms) {
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        frame->resize(checked_cast(natoms));
    )
}

extern "C" chfl_status chfl_frame_add_velocities(CHFL_FRAME* const frame) {
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        frame->add_velocities();
    )
}

extern "C" chfl_status chfl_frame_has_velocities(const CHFL_FRAME* const frame, bool* has_velocities) {
    assert(frame != nullptr);
    assert(has_velocities != nullptr);
    CHFL_ERROR_CATCH(
        *has_velocities = bool(frame->velocities());
    )
}

extern "C" chfl_status chfl_frame_set_cell(CHFL_FRAME* const frame, const CHFL_CELL* const cell) {
    assert(frame != nullptr);
    assert(cell != nullptr);
    CHFL_ERROR_CATCH(
        frame->set_cell(*cell);
    )
}

extern "C" chfl_status chfl_frame_set_topology(CHFL_FRAME* const frame, const CHFL_TOPOLOGY* const topology) {
    assert(frame != nullptr);
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        frame->set_topology(*topology);
    )
}

extern "C" chfl_status chfl_frame_step(const CHFL_FRAME* const frame, uint64_t* step) {
    assert(frame != nullptr);
    assert(step != nullptr);
    CHFL_ERROR_CATCH(
        *step = frame->step();
    )
}

extern "C" chfl_status chfl_frame_set_step(CHFL_FRAME* const frame, uint64_t step) {
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        frame->set_step(checked_cast(step));
    )
}

extern "C" chfl_status chfl_frame_guess_topology(CHFL_FRAME* const frame) {
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        frame->guess_topology();
    )
}

extern "C" chfl_status chfl_frame_free(CHFL_FRAME* const frame) {
    delete frame;
    return CHFL_SUCCESS;
}
