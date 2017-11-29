// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/capi/trajectory.h"
#include "chemfiles/capi.hpp"

#include "chemfiles/Trajectory.hpp"
using namespace chemfiles;

extern "C" CHFL_TRAJECTORY* chfl_trajectory_open(const char* path, char mode) {
    CHFL_TRAJECTORY* trajectory = nullptr;
    CHECK_POINTER_GOTO(path);
    CHFL_ERROR_GOTO(
        trajectory = new Trajectory(path, mode);
    )
    return trajectory;
error:
    delete trajectory;
    return nullptr;
}

extern "C" CHFL_TRAJECTORY* chfl_trajectory_with_format(const char* path, char mode, const char* format) {
    CHFL_TRAJECTORY* trajectory = nullptr;
    CHECK_POINTER_GOTO(path);
    CHECK_POINTER_GOTO(format);
    CHFL_ERROR_GOTO(
        trajectory = new Trajectory(path, mode, format);
    )
    return trajectory;
error:
    delete trajectory;
    return nullptr;
}

extern "C" chfl_status chfl_trajectory_read_step(CHFL_TRAJECTORY* const trajectory, uint64_t step, CHFL_FRAME* const frame) {
    CHECK_POINTER(trajectory);
    CHECK_POINTER(frame);
    CHFL_ERROR_CATCH(
        *frame = trajectory->read_step(checked_cast(step));
    )
}

extern "C" chfl_status chfl_trajectory_read(CHFL_TRAJECTORY* const trajectory, CHFL_FRAME* const frame) {
    CHECK_POINTER(trajectory);
    CHECK_POINTER(frame);
    CHFL_ERROR_CATCH(
        *frame = trajectory->read();
    )
}

extern "C" chfl_status chfl_trajectory_write(CHFL_TRAJECTORY* const trajectory, const CHFL_FRAME* const frame) {
    CHECK_POINTER(trajectory);
    CHECK_POINTER(frame);
    CHFL_ERROR_CATCH(
         trajectory->write(*frame);
    )
}

extern "C" chfl_status chfl_trajectory_set_topology(CHFL_TRAJECTORY* const trajectory, const CHFL_TOPOLOGY* const topology) {
    CHECK_POINTER(trajectory);
    CHECK_POINTER(topology);
    CHFL_ERROR_CATCH(
        trajectory->set_topology(*topology);
    )
}

extern "C" chfl_status chfl_trajectory_topology_file(CHFL_TRAJECTORY* const trajectory, const char* path, const char* format) {
    CHECK_POINTER(trajectory);
    CHECK_POINTER(path);
    CHFL_ERROR_CATCH(
        if (format == nullptr) {
            format = "";
        }
        trajectory->set_topology(path, format);
    )
}

extern "C" chfl_status chfl_trajectory_set_cell(CHFL_TRAJECTORY* const trajectory, const CHFL_CELL* const cell) {
    CHECK_POINTER(trajectory);
    CHECK_POINTER(cell);
    CHFL_ERROR_CATCH(
        trajectory->set_cell(*cell);
    )
}

extern "C" chfl_status chfl_trajectory_nsteps(CHFL_TRAJECTORY* const trajectory, uint64_t* nsteps) {
    CHECK_POINTER(trajectory);
    CHECK_POINTER(nsteps);
    CHFL_ERROR_CATCH(
        *nsteps = trajectory->nsteps();
    )
}

extern "C" chfl_status chfl_trajectory_close(CHFL_TRAJECTORY* trajectory) {
    delete trajectory;
    return CHFL_SUCCESS;
}
