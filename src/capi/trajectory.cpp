// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/capi/trajectory.h"
#include "chemfiles/capi.hpp"

#include "chemfiles/Trajectory.hpp"
using namespace chemfiles;

extern "C" CHFL_TRAJECTORY* chfl_trajectory_open(const char* filename, char mode) {
    assert(filename != nullptr);
    CHFL_TRAJECTORY* trajectory = nullptr;
    CHFL_ERROR_GOTO(
        trajectory = new Trajectory(filename, mode);
    )
    return trajectory;
error:
    delete trajectory;
    return nullptr;
}

extern "C" CHFL_TRAJECTORY* chfl_trajectory_with_format(const char* filename, char mode, const char* format) {
    assert(filename != nullptr);
    assert(format != nullptr);
    CHFL_TRAJECTORY* trajectory = nullptr;
    CHFL_ERROR_GOTO(
        trajectory = new Trajectory(filename, mode, format);
    )
    return trajectory;
error:
    delete trajectory;
    return nullptr;
}

extern "C" chfl_status chfl_trajectory_read_step(CHFL_TRAJECTORY* const file, uint64_t step, CHFL_FRAME* const frame) {
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        *frame = file->read_step(checked_cast(step));
    )
}

extern "C" chfl_status chfl_trajectory_read(CHFL_TRAJECTORY* const file, CHFL_FRAME* const frame) {
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
        *frame = file->read();
    )
}

extern "C" chfl_status chfl_trajectory_write(CHFL_TRAJECTORY* const file, const CHFL_FRAME* const frame) {
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_CATCH(
         file->write(*frame);
    )
}

extern "C" chfl_status chfl_trajectory_set_topology(CHFL_TRAJECTORY* const file, const CHFL_TOPOLOGY* const topology) {
    assert(file != nullptr);
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        file->set_topology(*topology);
    )
}

extern "C" chfl_status chfl_trajectory_topology_file(CHFL_TRAJECTORY* const file, const char* filename, const char* format) {
    assert(file != nullptr);
    assert(filename != nullptr);
    CHFL_ERROR_CATCH(
        if (format == nullptr) {
            format = "";
        }
        file->set_topology(std::string(filename), std::string(format));
    )
}

extern "C" chfl_status chfl_trajectory_set_cell(CHFL_TRAJECTORY* const file, const CHFL_CELL* const cell) {
    assert(file != nullptr);
    assert(cell != nullptr);
    CHFL_ERROR_CATCH(
        file->set_cell(*cell);
    )
}

extern "C" chfl_status chfl_trajectory_nsteps(CHFL_TRAJECTORY* const file, uint64_t* nsteps) {
    assert(file != nullptr);
    assert(nsteps != nullptr);
    CHFL_ERROR_CATCH(
        *nsteps = file->nsteps();
    )
}

extern "C" chfl_status chfl_trajectory_close(CHFL_TRAJECTORY* file) {
    delete file;
    return CHFL_SUCCESS;
}
