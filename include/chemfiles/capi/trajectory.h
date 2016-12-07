// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_TRAJECTORY_H
#define CHEMFILES_CHFL_TRAJECTORY_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// @brief Open a trajectory file.
/// @param filename The path to the trajectory file
/// @param mode The opening mode: 'r' for read, 'w' for write and 'a' for append.
/// @return A pointer to the file, or NULL in case of error
CHFL_EXPORT CHFL_TRAJECTORY* chfl_trajectory_open(const char* filename,
                                                  char mode);

/// @brief Open a trajectory file using a specific file format.
///
/// This can be needed when the file format does not match the extension, or
/// when there is not standard extension for this format.
///
/// @param filename The path to the trajectory file
/// @param mode The opening mode: 'r' for read, 'w' for write and 'a' for append.
/// @param format The file format to use. An empty string means that the format
///               should be guessed from the extension.
/// @return A pointer to the file, or NULL in case of error
CHFL_EXPORT CHFL_TRAJECTORY* chfl_trajectory_with_format(const char* filename,
                                                         char mode,
                                                         const char* format);

/// @brief Read the next step of the trajectory into a frame
/// @param file A pointer to the trajectory
/// @param frame A frame to fill with the data
/// @return The status code.
CHFL_EXPORT chfl_status chfl_trajectory_read(CHFL_TRAJECTORY*const file,
                                             CHFL_FRAME* const frame);

/// @brief Read a specific step of the trajectory in a frame
/// @param file A pointer to the trajectory
/// @param step The step to read
/// @param frame A frame to fill with the data
/// @return The status code.
CHFL_EXPORT chfl_status chfl_trajectory_read_step(CHFL_TRAJECTORY* const file,
                                                  uint64_t step,
                                                  CHFL_FRAME* const frame);

/// @brief Write a frame to the trajectory.
/// @param file The trajectory to write
/// @param frame the frame which will be writen to the file
/// @return The status code.
CHFL_EXPORT chfl_status chfl_trajectory_write(CHFL_TRAJECTORY* const file,
                                              const CHFL_FRAME* const frame);

/// @brief Set the topology associated with a trajectory. This topology will be
///        used when reading and writing the files, replacing any topology in the
///        frames or files.
/// @param file A pointer to the trajectory
/// @param topology The new topology to use
/// @return The status code.
CHFL_EXPORT chfl_status chfl_trajectory_set_topology(
                                                CHFL_TRAJECTORY* const file,
                                                const CHFL_TOPOLOGY* const topology);

/// @brief Set the topology associated with a trajectory by reading the first
///        frame of `filename`; and extracting the topology of this frame.
/// @param file A pointer to the trajectory
/// @param filename The file to read in order to get the new topology
/// @return The status code.
CHFL_EXPORT chfl_status chfl_trajectory_set_topology_file(
                                                CHFL_TRAJECTORY* const file,
                                                const char* filename);

/// @brief Set the topology associated with a trajectory by reading the first
///        frame of `filename` using the file format in `format`; and
///        extracting the topology of this frame.
///
/// This can be needed when the topology file format does not match the extension, or
/// when there is not standard extension for this format.
///
/// @param file A pointer to the trajectory
/// @param filename The file to read in order to get the new topology
/// @param format The name of the file format to use for reading the topology. An
///               empty string means that the format should be guessed from the
///               extension.
/// @return The status code.
CHFL_EXPORT chfl_status chfl_trajectory_set_topology_with_format(
                                                CHFL_TRAJECTORY* const file,
                                                const char* filename,
                                                const char* format);

/// @brief Set the unit cell associated with a trajectory. This cell will be
///        used when reading and writing the files, replacing any unit cell in the
///        frames or files.
/// @param file A pointer to the trajectory
/// @param cell The new cell to use
/// @return The status code.
CHFL_EXPORT chfl_status chfl_trajectory_set_cell(CHFL_TRAJECTORY* const file,
                                                 const CHFL_CELL* const cell);

/// @brief Get the number of steps (the number of frames) in a trajectory.
/// @param file A pointer to the trajectory
/// @param nsteps This will contain the number of steps
/// @return The status code.
CHFL_EXPORT chfl_status chfl_trajectory_nsteps(CHFL_TRAJECTORY* const file,
                                               uint64_t* nsteps);

/// @brief Close a trajectory file, and free the associated memory
/// @param file A pointer to the file
/// @return The status code
CHFL_EXPORT chfl_status chfl_trajectory_close(CHFL_TRAJECTORY* file);

#ifdef __cplusplus
}
#endif

#endif
