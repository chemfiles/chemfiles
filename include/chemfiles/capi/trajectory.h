// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_TRAJECTORY_H
#define CHEMFILES_CHFL_TRAJECTORY_H

#include <stdint.h>

#include "chemfiles/capi/types.h"
#include "chemfiles/exports.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Open the file at the given `path` using the given `mode`.
///
/// Valid modes are `'r'` for read, `'w'` for write and `'a'` for append.
///
/// The caller of this function should free the allocated memory using
/// `chfl_trajectory_close`.
///
/// @example{capi/chfl_trajectory/open.c}
/// @return A pointer to the trajectory, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_TRAJECTORY* chfl_trajectory_open(const char* path, char mode);

/// Open the file at the given `path` using a specific file `format` and the
/// given `mode`.
///
/// Valid modes are `'r'` for read, `'w'` for write and `'a'` for append.
///
/// The `format` parameter is needed when the file format does not match the
/// extension, or when there is not standard extension for this format. If
/// `format` is an empty string, the format will be guessed from the extension.
///
/// The caller of this function should free the allocated memory using
/// `chfl_trajectory_close`.
///
/// @example{capi/chfl_trajectory/with_format.c}
/// @return A pointer to the trajectory, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_TRAJECTORY* chfl_trajectory_with_format(
    const char* path, char mode, const char* format
);

/// Get the path used to open the `trajectory` in `path`.
///
/// The `path` will point to memory allocated inside the `trajectory`, and it is
/// only valid until the trajectory is closed (`chfl_trajectory_close`). There
/// is no need to `free` the corresponding memory
///
/// @example{capi/chfl_trajectory/path.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_trajectory_path(
    const CHFL_TRAJECTORY* trajectory, const char** path
);

/// Read the next step of the `trajectory` into a `frame`.
///
/// If the number of atoms in frame does not correspond to the number of atom
/// in the next step, the frame is resized.
///
/// @example{capi/chfl_trajectory/read.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_trajectory_read(
    CHFL_TRAJECTORY* trajectory, CHFL_FRAME* frame
);

/// Read a specific `step` of the `trajectory` into a `frame`.
///
/// If the number of atoms in frame does not correspond to the number of atom
/// in the step, the frame is resized.
///
/// @example{capi/chfl_trajectory/read_step.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_trajectory_read_step(
    CHFL_TRAJECTORY* trajectory, uint64_t step, CHFL_FRAME* frame
);

/// Write a single `frame` to the `trajectory`.
///
/// @example{capi/chfl_trajectory/write.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_trajectory_write(
    CHFL_TRAJECTORY* trajectory, const CHFL_FRAME* frame
);

/// Set the `topology` associated with a `trajectory`. This topology will be
/// used when reading and writing the files, replacing any topology in the
/// frames or files.
///
/// @example{capi/chfl_trajectory/set_topology.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_trajectory_set_topology(
    CHFL_TRAJECTORY* trajectory, const CHFL_TOPOLOGY* topology
);

/// Set the topology associated with a `trajectory` by reading the first frame
/// of the file at the given `path` using the file format in `format`; and
/// extracting the topology of this frame.
///
/// If `format` is an empty string or `NULL`, the format will be guessed from
/// the path extension.
///
/// @example{capi/chfl_trajectory/topology_file.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_trajectory_topology_file(
    CHFL_TRAJECTORY* trajectory, const char* path, const char* format
);

/// Set the unit `cell` associated with a `trajectory`. This cell will be used
/// when reading and writing the files, replacing any pre-existing unit cell.
///
/// @example{capi/chfl_trajectory/set_cell.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_trajectory_set_cell(
    CHFL_TRAJECTORY* trajectory, const CHFL_CELL* cell
);

/// Store the number of steps (the number of frames) from the `trajectory` in
/// `nsteps`.
///
/// @example{capi/chfl_trajectory/nsteps.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_trajectory_nsteps(
    CHFL_TRAJECTORY* trajectory, uint64_t* nsteps
);

/// Close a trajectory file, and free the associated memory.
///
/// Closing a file will synchronize all changes made to the file with the
/// storage (hard drive, network, ...) used for this file.
///
/// @example{capi/chfl_trajectory/open.c}
CHFL_EXPORT void chfl_trajectory_close(const CHFL_TRAJECTORY* trajectory);

#ifdef __cplusplus
}
#endif

#endif
