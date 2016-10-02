/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

/*! @file chemfiles.h
* Chemfiles C interface header.
*
* This file contains all the function definitions for the C API of chemfiles,
* and
* should be self-documented enough.
*/

#ifndef CHEMFILES_CAPI_H
#define CHEMFILES_CAPI_H

#define CHEMFILES_PUBLIC
#include "chemfiles/config.hpp"
#undef CHEMFILES_PUBLIC

// clang-format off
#ifdef __cplusplus
    #include <cstddef>
    extern "C" {
    namespace chemfiles {
        class Trajectory;
        class Frame;
        class Atom;
        class UnitCell;
        class Topology;
        class Residue;
    }
    struct CAPISelection;
    typedef chemfiles::Trajectory CHFL_TRAJECTORY;
    typedef chemfiles::Frame CHFL_FRAME;
    typedef chemfiles::Atom CHFL_ATOM;
    typedef chemfiles::UnitCell CHFL_CELL;
    typedef chemfiles::Topology CHFL_TOPOLOGY;
    typedef chemfiles::Residue CHFL_RESIDUE;
    typedef CAPISelection CHFL_SELECTION;
#else
    #include <stdbool.h>
    #include <stddef.h>
    //! Opaque type handling trajectories files
    typedef struct CHFL_TRAJECTORY CHFL_TRAJECTORY;
    //! Opaque type handling frames, *i.e* data from a step
    typedef struct CHFL_FRAME CHFL_FRAME;
    //! Opaque type handling an atom
    typedef struct CHFL_ATOM CHFL_ATOM;
    //! Opaque type handling an unit cell
    typedef struct CHFL_CELL CHFL_CELL;
    //! Opaque type handling a topology
    typedef struct CHFL_TOPOLOGY CHFL_TOPOLOGY;
    //! Opaque type handling a residue
    typedef struct CHFL_RESIDUE CHFL_RESIDUE;
    //! Opaque type handling a selection
    typedef struct CHFL_SELECTION CHFL_SELECTION;
#endif
// clang-format on

typedef enum {
    //! Status code for success
    CHFL_SUCCESS = 0,
    //! Memory error: out of memory, wrong size for arrays parameters, ...
    CHFL_MEMORY_ERROR = 1,
    //! File error: file do not exist, you do not have rights to open it, ...
    CHFL_FILE_ERROR = 2,
    //! Error in file formating
    CHFL_FORMAT_ERROR = 3,
    //! Error in selection parsing
    CHFL_SELECTION_ERROR = 4,
    //! Any other error from Chemfiles
    CHFL_GENERIC_ERROR = 5,
    //! Error in the C++ standard library
    CHFL_CXX_ERROR = 6
} chfl_status;

typedef float chfl_vector_t[3];

/*!
* @brief Get the version of the chemfiles library
* @return A null-terminated string containing the version of Chemfiles.
*/
CHFL_EXPORT const char* chfl_version(void);

/*!
* @brief Get the error message corresponding to an error code.
* @param code The status code
* @return A null-terminated string encoding the textual representation of the
*         status.
*/
CHFL_EXPORT const char* chfl_strerror(chfl_status code);

/*!
* @brief Get the last error message.
* @return A null-terminated string encoding the textual representation of the
*         last error.
*/
CHFL_EXPORT const char* chfl_last_error(void);

/*!
* @brief Clear the last error message.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_clear_errors(void);

//! Available logging level
typedef enum CHFL_LOG_LEVEL {
    //! Only log on errors
    CHFL_LOG_ERROR = 0,
    //! Log warnings and erors
    CHFL_LOG_WARNING = 1,
    //! Log infos, warnings and errors
    CHFL_LOG_INFO = 2,
    //! Log everything
    CHFL_LOG_DEBUG = 3
} chfl_log_level_t;

/*!
* @brief Get the current maximal logging level
* @param level The logging level
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_loglevel(chfl_log_level_t* const level);

/*!
* @brief Set the current maximal logging level to `level`
* @param level The new logging level
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_set_loglevel(chfl_log_level_t level);

/*!
* @brief Redirect the logs to `file`, overwriting the file if it exists
* @param file The filename for the new log file.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_logfile(const char* file);

/*!
* @brief Redirect the logs to the standard output.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_log_stdout(void);

/*!
* @brief Redirect the logs to the standard error output. This is enabled by
*        default.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_log_stderr(void);

/*!
* @brief Remove all logging output
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_log_silent(void);

//! Callback function type for user-provided logging
typedef void (*chfl_logging_cb)(chfl_log_level_t level, const char* message);

/*!
* @brief Redirect all logging to user-provided logging. The `callback` function
* will be called at each loggin operation with the level of the message, and
* the message itself.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_log_callback(chfl_logging_cb callback);

/******************************************************************************/
/*!
* @brief Open a trajectory file.
* @param filename The path to the trajectory file
* @param mode The opening mode: 'r' for read, 'w' for write and 'a' for append.
* @return A pointer to the file, or NULL in case of error
*/
CHFL_EXPORT CHFL_TRAJECTORY* chfl_trajectory_open(const char* filename,
                                                  char mode);

/*!
* @brief Open a trajectory file using a specific file format.
*
* This can be needed when the file format does not match the extension, or
* when there is not standard extension for this format.
*
* @param filename The path to the trajectory file
* @param mode The opening mode: 'r' for read, 'w' for write and 'a' for append.
* @param format The file format to use. An empty string means that the format
*               should be guessed from the extension.
* @return A pointer to the file, or NULL in case of error
*/
CHFL_EXPORT CHFL_TRAJECTORY* chfl_trajectory_with_format(const char* filename,
                                                         char mode,
                                                         const char* format);

/*!
* @brief Read the next step of the trajectory into a frame
* @param file A pointer to the trajectory
* @param frame A frame to fill with the data
* @return The status code.
*/
CHFL_EXPORT chfl_status chfl_trajectory_read(CHFL_TRAJECTORY*const file,
                                             CHFL_FRAME* const frame);

/*!
* @brief Read a specific step of the trajectory in a frame
* @param file A pointer to the trajectory
* @param step The step to read
* @param frame A frame to fill with the data
* @return The status code.
*/
CHFL_EXPORT chfl_status chfl_trajectory_read_step(CHFL_TRAJECTORY* const file,
                                                  size_t step,
                                                  CHFL_FRAME* const frame);

/*!
* @brief Write a frame to the trajectory.
* @param file The trajectory to write
* @param frame the frame which will be writen to the file
* @return The status code.
*/
CHFL_EXPORT chfl_status chfl_trajectory_write(CHFL_TRAJECTORY* const file,
                                              const CHFL_FRAME* const frame);

/*!
* @brief Set the topology associated with a trajectory. This topology will be
*        used when reading and writing the files, replacing any topology in the
*        frames or files.
* @param file A pointer to the trajectory
* @param topology The new topology to use
* @return The status code.
*/
CHFL_EXPORT chfl_status chfl_trajectory_set_topology(
                                                CHFL_TRAJECTORY* const file,
                                                const CHFL_TOPOLOGY* const topology);

/*!
* @brief Set the topology associated with a trajectory by reading the first
*        frame of `filename`; and extracting the topology of this frame.
* @param file A pointer to the trajectory
* @param filename The file to read in order to get the new topology
* @return The status code.
*/
CHFL_EXPORT chfl_status chfl_trajectory_set_topology_file(
                                                CHFL_TRAJECTORY* const file,
                                                const char* filename);

/*!
* @brief Set the topology associated with a trajectory by reading the first
*        frame of `filename` using the file format in `format`; and
*        extracting the topology of this frame.
*
* This can be needed when the topology file format does not match the extension, or
* when there is not standard extension for this format.
*
* @param file A pointer to the trajectory
* @param filename The file to read in order to get the new topology
* @param format The name of the file format to use for reading the topology. An
*               empty string means that the format should be guessed from the
*               extension.
* @return The status code.
*/
CHFL_EXPORT chfl_status chfl_trajectory_set_topology_with_format(
                                                CHFL_TRAJECTORY* const file,
                                                const char* filename,
                                                const char* format);

/*!
* @brief Set the unit cell associated with a trajectory. This cell will be
*        used when reading and writing the files, replacing any unit cell in the
*        frames or files.
* @param file A pointer to the trajectory
* @param cell The new cell to use
* @return The status code.
*/
CHFL_EXPORT chfl_status chfl_trajectory_set_cell(CHFL_TRAJECTORY* const file,
                                                 const CHFL_CELL* const cell);

/*!
* @brief Get the number of steps (the number of frames) in a trajectory.
* @param file A pointer to the trajectory
* @param nsteps This will contain the number of steps
* @return The status code.
*/
CHFL_EXPORT chfl_status chfl_trajectory_nsteps(CHFL_TRAJECTORY* const file,
                                               size_t* nsteps);

/*!
* @brief Close a trajectory file, and free the associated memory
* @param file A pointer to the file
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_trajectory_close(CHFL_TRAJECTORY* file);

/******************************************************************************/
/*!
* @brief Create an empty frame with initial capacity of `natoms`. It will be
*        resized by the library as needed.
* @param natoms the size of the wanted frame
* @return A pointer to the frame, or NULL in case of error
*/
CHFL_EXPORT CHFL_FRAME* chfl_frame(size_t natoms);

/*!
* @brief Get the current number of atoms in the frame.
* @param frame The frame to analyse
* @param natoms the number of atoms in the frame
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_atoms_count(const CHFL_FRAME* const frame,
                                               size_t* natoms);

/*!
* @brief Get a pointer to the positions array from a frame.
*
* The positions are stored as a N x 3 array, this function set a pointer to
* point to the first element of this array, and give the value of N. If the
* frame is resized (by writing to it, or calling `chfl_frame_resize`), the
* pointer is invalidated.
*
* @param frame The frame
* @param data A pointer to a `chfl_vector_t` array, which will be set to point
*             to the positions data.
* @param size A pointer to the an integer to be filled with the array size
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_positions(CHFL_FRAME* const frame,
                                             chfl_vector_t** data,
                                             size_t* size);

/*!
* @brief Get a pointer to the velocities array from a frame.
*
* The velocities are stored as a `N x 3` array, this function set a pointer
* to point to the first element of this array, and give the value of N. If the
* frame is resized (by writing to it, or calling `chfl_frame_resize`), the
* pointer is invalidated.
*
* If the frame do not have velocity, this will return an error. Use
* `chfl_frame_add_velocities` to add velocities to a frame before calling
* this function.
*
* @param frame The frame
* @param data A pointer to a `chfl_vector_t` array, which will be set to point
*             to the velocity data.
* @param size A pointer to the an integer to be filled with the array size
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_velocities(CHFL_FRAME* const frame,
                                              chfl_vector_t** data,
                                              size_t* size);

/*!
* @brief Add an atom and the corresponding position (and velocity) data to a
*        frame.
*
* @param frame The frame
* @param atom The atom to add
* @param position The position of the atom
* @param velocity The velocity of the atom. This parameter can be `NULL` if no
*                 velocity is associated with the atom.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_add_atom(CHFL_FRAME* const frame,
                                            const CHFL_ATOM* const atom,
                                            chfl_vector_t position,
                                            chfl_vector_t velocity);

/*!
* @brief Resize the positions and the velocities in frame, to make space for
*        `natoms` atoms.
*
* This function may invalidate any pointer to the positions or the
* velocities if the new size is bigger than the old one. In all the cases,
* previous data is conserved. This function conserve the presence of absence of
* velocities.
*
* @param frame The frame
* @param natoms The new number of atoms.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_resize(CHFL_FRAME* const frame, size_t natoms);

/*!
* @brief Add velocity storage to this frame.
*
* The storage is initialized with the result of `chfl_frame_atoms_count` as
* number of atoms. If the frame already have velocities, this does nothing.
*
* @param frame The frame
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_add_velocities(CHFL_FRAME* const frame);

/*!
* @brief Ask wether this frame contains velocity data or not.
* @param frame The frame
* @param has_velocities A boolean, will be true if the frame have velocities,
*                       false otherwise.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_has_velocities(const CHFL_FRAME* const frame,
                                                  bool* has_velocities);

/*!
* @brief Set the UnitCell of a Frame.
* @param frame The frame
* @param cell The new cell
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_set_cell(CHFL_FRAME* const frame,
                                            const CHFL_CELL* const cell);

/*!
* @brief Set the Topology of a Frame.
* @param frame The frame
* @param topology The new topology
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_set_topology(CHFL_FRAME* const frame,
                                                const CHFL_TOPOLOGY* const topology);

/*!
* @brief Get the Frame step, i.e. the frame number in the trajectory
* @param frame The frame
* @param step This will contains the step number
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_step(const CHFL_FRAME* const frame, size_t* step);

/*!
* @brief Set the Frame step.
* @param frame The frame
* @param step The new frame step
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_set_step(CHFL_FRAME* const frame, size_t step);

/*!
* @brief  Guess the bonds, angles and dihedrals in the system.
*
* The bonds are guessed using a distance-based algorithm, and then angles and
* dihedrals are guessed from the bonds.
*
* @param frame The Frame to analyse
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_guess_topology(CHFL_FRAME* const frame);

/*!
* @brief Destroy a frame, and free the associated memory
* @param frame The frame to destroy
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_frame_free(CHFL_FRAME* frame);

/******************************************************************************/
/*!
* @brief Create an ORTHORHOMBIC UnitCell from the three lenghts
* @param a first lenght of the cell (in Angstroms)
* @param b second lenght of the cell (in Angstroms)
* @param c third lenght of the cell (in Angstroms)
* @return A pointer to the UnitCell, or NULL in case of error
*/
CHFL_EXPORT CHFL_CELL* chfl_cell(double a, double b, double c);

/*!
* @brief Create a TRICLINIC UnitCell from the three lenghts and the three angles
* @param a first lenght of the cell (in Angstroms)
* @param b second lenght of the cell (in Angstroms)
* @param c third lenght of the cell (in Angstroms)
* @param alpha angle of the cell between the vectors `b` and `c` (in degree)
* @param beta angle of the cell between the vectors `a` and `c` (in degree)
* @param gamma angle of the cell between the vectors `a` and `b` (in degree)
* @return A pointer to the UnitCell, or NULL in case of error
*/
CHFL_EXPORT CHFL_CELL* chfl_cell_triclinic(double a,
                                           double b,
                                           double c,
                                           double alpha,
                                           double beta,
                                           double gamma);

/*!
* @brief Get the UnitCell from a frame
* @param frame the frame
* @return A pointer to the UnitCell, or NULL in case of error
*/
CHFL_EXPORT CHFL_CELL* chfl_cell_from_frame(const CHFL_FRAME* const frame);

/*!
* @brief Get the cell volume.
* @param cell the unit cell to read
* @param volume the volume
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_volume(const CHFL_CELL* const cell, double* volume);

/*!
* @brief Get the cell lenghts.
* @param cell the unit cell to read
* @param a first lenght of the cell (in Angstroms)
* @param b second lenght of the cell (in Angstroms)
* @param c third lenght of the cell (in Angstroms)
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_lengths(const CHFL_CELL* const cell,
                                          double* a,
                                          double* b,
                                          double* c);

/*!
* @brief Set the unit cell lenghts.
* @param cell the unit cell to modify
* @param a first lenght of the cell (in Angstroms)
* @param b second lenght of the cell (in Angstroms)
* @param c third lenght of the cell (in Angstroms)
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_set_lengths(CHFL_CELL* const cell,
                                              double a,
                                              double b,
                                              double c);

/*!
* @brief Get the cell angles, in degrees.
* @param cell the cell to read
* @param alpha angle of the cell between the vectors `b` and `c` (in degree)
* @param beta angle of the cell between the vectors `a` and `c` (in degree)
* @param gamma angle of the cell between the vectors `a` and `b` (in degree)
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_angles(const CHFL_CELL* const cell,
                                        double* alpha,
                                        double* beta,
                                        double* gamma);

/*!
* @brief Set the cell angles, in degrees. This is only possible for TRICLINIC
* cells.
* @param cell the unit cell to modify
* @param alpha angle of the cell between the vectors `b` and `c` (in degree)
* @param beta angle of the cell between the vectors `a` and `c` (in degree)
* @param gamma angle of the cell between the vectors `a` and `b` (in degree)
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_set_angles(CHFL_CELL* const cell,
                                             double alpha,
                                             double beta,
                                             double gamma);

/*!
* @brief Get the unit cell matricial representation.
* @param cell the unit cell to use
* @param matrix the matrix to fill. It should be a 3x3 matrix.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_matrix(const CHFL_CELL* const cell,
                                         double (*matrix)[3]);

//! Available cell shapes in chemfiles
typedef enum CHFL_CELL_SHAPE {
    //! The three angles are 90°
    CHFL_CELL_ORTHORHOMBIC = 0,
    //! The three angles may not be 90°
    CHFL_CELL_TRICLINIC = 1,
    //! Cell type when there is no periodic boundary conditions
    CHFL_CELL_INFINITE = 2,
} chfl_cell_shape_t;

/*!
* @brief Get the cell shape
* @param cell the unit cell to read
* @param shape the shape of the cell
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_shape(const CHFL_CELL* const cell,
                                       chfl_cell_shape_t* const shape);

/*!
* @brief Set the cell shape
* @param cell the cell to modify
* @param shape the new shape of the cell
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_set_shape(CHFL_CELL* const cell,
                                           chfl_cell_shape_t shape);

/*!
* @brief Destroy an unit cell, and free the associated memory
* @param cell The cell to destroy
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_cell_free(CHFL_CELL* cell);

/******************************************************************************/

/*!
* @brief Create a new empty topology
* @return A pointer to the new topology, or NULL in case of error
*/
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology(void);

/*!
* @brief Get a copy of the topology of a frame
* @param frame The frame
* @return A pointer to the new topology, or NULL in case of error
*/
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology_from_frame(const CHFL_FRAME* const frame);

/*!
* @brief Get the current number of atoms in the topology.
* @param topology The topology to analyse
* @param natoms Will contain the number of atoms in the frame
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_atoms_count(const CHFL_TOPOLOGY* const topology,
                                                  size_t* natoms);

/*!
* @brief Resize the topology to hold `natoms` atoms, adding `CHFL_ATOM_UNDEFINED`
*        atoms as needed.
*
* @param topology The topology
* @param natoms The new number of atoms.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_resize(CHFL_TOPOLOGY* const topology, size_t natoms);

/*!
* @brief Add an atom at the end of a topology
* @param topology The topology
* @param atom The atom to be added
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_append(CHFL_TOPOLOGY* const topology,
                                             const CHFL_ATOM* const atom);

/*!
* @brief Remove an atom from a topology by index. This modify all the other
* atoms indexes.
* @param topology The topology
* @param i The atomic index
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_remove(CHFL_TOPOLOGY* const topology, size_t i);

/*!
* @brief Tell if the atoms `i` and `j` are bonded together
* @param topology The topology
* @param i index of the first atom in the topology
* @param j index of the second atom in the topology
* @param result true if the atoms are bonded, false otherwise
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_isbond(const CHFL_TOPOLOGY* const topology,
                                            size_t i,
                                            size_t j,
                                            bool* result);

/*!
* @brief Tell if the atoms `i`, `j` and `k` constitues an angle
* @param topology The topology
* @param i index of the first atom in the topology
* @param j index of the second atom in the topology
* @param k index of the third atom in the topology
* @param result true if the atoms constitues an angle, false otherwise
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_isangle(const CHFL_TOPOLOGY* const topology,
                                              size_t i,
                                              size_t j,
                                              size_t k,
                                              bool* result);

/*!
* @brief Tell if the atoms `i`, `j`, `k` and `m` constitues a dihedral angle
* @param topology The topology
* @param i index of the first atom in the topology
* @param j index of the second atom in the topology
* @param k index of the third atom in the topology
* @param m index of the fourth atom in the topology
* @param result true if the atoms constitues a dihedral angle, false otherwise
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_isdihedral(const CHFL_TOPOLOGY* const topology,
                                                 size_t i,
                                                 size_t j,
                                                 size_t k,
                                                 size_t m,
                                                 bool* result);

/*!
* @brief Get the number of bonds in the system
* @param topology The topology
* @param nbonds After the call, contains the number of bond
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_bonds_count(const CHFL_TOPOLOGY* const topology,
                                                  size_t* nbonds);

/*!
* @brief Get the number of angles in the system
* @param topology The topology
* @param nangles After the call, contains the number of angles
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_angles_count(const CHFL_TOPOLOGY* const topology,
                                                   size_t* nangles);

/*!
* @brief Get the number of dihedral angles in the system
* @param topology The topology
* @param ndihedrals After the call, contains the number of dihedral angles
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_dihedrals_count(
                                            const CHFL_TOPOLOGY* const topology,
                                            size_t* ndihedrals);

/*!
* @brief Get the list of bonds in the system
* @param topology The topology
* @param data A nbonds x 2 array to be filled with the bonds in the system
* @param nbonds The size of the array. This should equal the value given by the
*               `chfl_topology_bonds_count` function
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_bonds(const CHFL_TOPOLOGY* const topology,
                                            size_t (*data)[2],
                                            size_t nbonds);

/*!
* @brief Get the list of angles in the system
* @param topology The topology
* @param data A nangles x 3 array to be filled with the angles in the system
* @param nangles The size of the array. This should equal the value given by the
*               `chfl_topology_angles_count` function
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_angles(const CHFL_TOPOLOGY* const topology,
                                             size_t (*data)[3],
                                             size_t nangles);

/*!
* @brief Get the list of dihedral angles in the system
* @param topology The topology
* @param data A ndihedrals x 4 array to be filled with the dihedral angles in
* the system
* @param ndihedrals The size of the array. This should equal the value given by
* the `chfl_topology_dihedrals_count` function
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_dihedrals(const CHFL_TOPOLOGY* const topology,
                                                size_t (*data)[4],
                                                size_t ndihedrals);

/*!
* @brief Add a bond between the atoms `i` and `j` in the system
* @param topology The topology
* @param i index of the first atom in the topology
* @param j index of the second atom in the topology
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_add_bond(CHFL_TOPOLOGY* const topology,
                                               size_t i,
                                               size_t j);

/*!
* @brief Remove any existing bond between the atoms `i` and `j` in the system
* @param topology The topology
* @param i index of the first atom in the topology
* @param j index of the second atom in the topology
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_remove_bond(CHFL_TOPOLOGY* const topology,
                                                  size_t i,
                                                  size_t j);

/*!
* @brief Get the number of residues in the system
* @param topology The topology
* @param residues The number of residues
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_residues_count(const CHFL_TOPOLOGY* const topology,
                                                     size_t* residues);

/*!
* @brief Add a residue to this topology
* @param topology The topology
* @param residue The new residue
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_add_residue(CHFL_TOPOLOGY* const topology,
                                                  const CHFL_RESIDUE* const residue);

/*!
* @brief Check if two residues are linked together, i.e. if there is a bond
*        between one atom in the first residue and one atom in the second one.
* @param topology The topology
* @param res_1 The first residue
* @param res_2 The second residue
* @param result true if the residues are linked, false otherwise
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_are_linked(CHFL_TOPOLOGY* const topology,
                                                 const CHFL_RESIDUE* const res_1,
                                                 const CHFL_RESIDUE* const res_2,
                                                 bool* result);

/*!
* @brief Destroy a topology, and free the associated memory
* @param topology The topology to destroy
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_topology_free(CHFL_TOPOLOGY* topology);

/******************************************************************************/

/*!
* @brief Create a new residue
* @param name The residue name
* @param resid The residue identifier, or `size_t(-1)` if the residue do not
*              have an identifier
* @return The status code
*/
CHFL_EXPORT CHFL_RESIDUE* chfl_residue(const char* name, size_t resid);

/*!
* @brief Get a residue from a topology
* @param topology The topology
* @param i The residue index in the topology. This is not always the same as
*          the `resid`. This value should be between 0 and the result of
*          `chfl_topology_residues_count`.
* @return The status code
*/
CHFL_EXPORT CHFL_RESIDUE* chfl_residue_from_topology(const CHFL_TOPOLOGY* const topology,
                                                     size_t i);

/*!
* @brief Get a the residue containing a given atom, or NULL if the atom is not
*        in a residue.
* @param topology The topology
* @param i The atom index
* @return The status code
*/
CHFL_EXPORT CHFL_RESIDUE* chfl_residue_for_atom(const CHFL_TOPOLOGY* const topology,
                                                size_t i);

/*!
* @brief Get the number of atoms in a residue
* @param residue The residue
* @param size The size of the residue
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_residue_atoms_count(const CHFL_RESIDUE* const residue,
                                                 size_t* size);

/*!
* @brief Get the identifier of a residue in the initial topology file
* @param residue The residue
* @param id The id of the residue
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_residue_id(const CHFL_RESIDUE* const residue,
                                        size_t* id);

/*!
* @brief Get the name of a residue
* @param residue The residue
* @param name A string buffer to be filled with the name
* @param buffsize The size of the string buffer
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_residue_name(const CHFL_RESIDUE* const residue,
                                          char* name,
                                          size_t buffsize);

/*!
* @brief Add the atom at index `i` in the residue
* @param residue The residue
* @param i The atomic index
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_residue_add_atom(CHFL_RESIDUE* const residue,
                                              size_t i);

/*!
* @brief Check if the atom at index `i` is in the residue
* @param residue The residue
* @param i The atomic index
* @param result true if the atom is in the residue, false otherwise
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_residue_contains(const CHFL_RESIDUE* const residue,
                                              size_t i,
                                              bool* result);

/*!
* @brief Destroy a residue, and free the associated memory
* @param residue The residue to destroy
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_residue_free(CHFL_RESIDUE* residue);

/******************************************************************************/

/*!
* @brief Create an atom from an atomic name
* @param name The new atom name
* @return A pointer to the new atom, or NULL in case of error
*/
CHFL_EXPORT CHFL_ATOM* chfl_atom(const char* name);

/*!
* @brief Get a specific atom from a frame
* @param frame The frame
* @param idx The atom index in the frame
* @return A pointer to the new atom, or NULL in case of error or if `idx`
*         is out of bounds
*/
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_frame(const CHFL_FRAME* const frame,
                                            size_t idx);

/*!
* @brief Get a specific atom from a topology
* @param topology The topology
* @param idx The atom index in the topology
* @return A pointer to the new atom, or NULL in case of error or if `idx`
*         is out of bounds
*/
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_topology(const CHFL_TOPOLOGY* const topology,
                                               size_t idx);

/*!
* @brief Get the mass of an atom, in atomic mass units
* @param atom The atom
* @param mass The atom mass
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_mass(const CHFL_ATOM* const atom, float* mass);

/*!
* @brief Set the mass of an atom, in atomic mass units
* @param atom The atom
* @param mass The new atom mass
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_set_mass(CHFL_ATOM* const atom, float mass);

/*!
* @brief Get the charge of an atom, in number of the electron charge e
* @param atom The atom
* @param charge The atom charge
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_charge(const CHFL_ATOM* const atom, float* charge);

/*!
* @brief Set the charge of an atom, in number of the electron charge e
* @param atom The atom
* @param charge The new atom charge
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_set_charge(CHFL_ATOM* const atom, float charge);

/*!
* @brief Get the name of an atom
* @param atom The atom
* @param name A string buffer to be filled with the name
* @param buffsize The size of the string buffer
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_name(const CHFL_ATOM* const atom,
                                       char* const name,
                                       size_t buffsize);

/*!
* @brief Set the name of an atom
* @param atom The atom
* @param name A null terminated string containing the new name
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_set_name(CHFL_ATOM* const atom, const char* name);

/*!
* @brief Try to get the full name of an atom from the short name
* @param atom The atom
* @param name A string buffer to be filled with the name
* @param buffsize The size of the string buffer
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_full_name(const CHFL_ATOM* const atom,
                                            char* const name,
                                            size_t buffsize);

/*!
* @brief Try to get the Van der Waals radius of an atom from the short name
* @param atom The atom
* @param radius The Van der Waals radius of the atom or -1 if no value could be
* found.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_vdw_radius(const CHFL_ATOM* const atom,
                                             double* radius);

/*!
* @brief Try to get the covalent radius of an atom from the short name
* @param atom The atom
* @param radius The covalent radius of the atom or -1 if no value could be
* found.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_covalent_radius(const CHFL_ATOM* const atom,
                                                  double* radius);

/*!
* @brief Try to get the atomic number of an atom from the short name
* @param atom The atom
* @param number The atomic number, or -1 if no value could be found.
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_atomic_number(const CHFL_ATOM* const atom,
                                                int* number);

//! Available types of atoms
typedef enum CHFL_ATOM_TYPES {
    //! Element from the periodic table of elements
    CHFL_ATOM_ELEMENT = 0,
    //! Coarse-grained atom are composed of more than one element: CH3 groups,
    //! amino-acids are coarse-grained atoms.
    CHFL_ATOM_COARSE_GRAINED = 1,
    //! Dummy site, with no physical reality
    CHFL_ATOM_DUMMY = 2,
    //! Undefined atom type
    CHFL_ATOM_UNDEFINED = 3,
} chfl_atom_type_t;

/*!
* @brief Get the atom type
* @param atom the atom to read
* @param type the type of the atom
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_type(const CHFL_ATOM* const atom,
                                       chfl_atom_type_t* const type);

/*!
* @brief Set the atom type
* @param atom the atom to modify
* @param type the new type of the atom
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_set_type(CHFL_ATOM* const atom,
                                           chfl_atom_type_t type);

/*!
* @brief Destroy an atom, and free the associated memory
* @param atom The atom to destroy
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_atom_free(CHFL_ATOM* atom);

/******************************************************************************/

/*!
* @brief Create a new selection from the given selection string.
* @return A pointer to the corresponding selection, or NULL in case of error
*/
CHFL_EXPORT CHFL_SELECTION* chfl_selection(const char* selection);

/*!
* @brief Get the size of the selection, i.e. the number of atoms we are
*        selecting together.
*
* This value is 1 for the 'atom' context, 2 for the 'pair' and 'bond' context,
* 3 for the 'three' and 'angles' contextes and 4 for the 'four' and 'dihedral'
* contextes.
*
* @param selection The selection
* @param size The size of the selection
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_selection_size(const CHFL_SELECTION* const selection,
                                            size_t* size);

/*!
* @brief Evaluate a selection for a given frame.
*
* Use the `chfl_selection_get` function to get the matches for this selection.
*
* @param selection The selection
* @param frame The frame
* @param n_matches The number of matches for the selection
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_selection_evalutate(CHFL_SELECTION* const selection,
                                                 const CHFL_FRAME* const frame,
                                                 size_t* n_matches);

//! Maximal size for a selection match
#define CHFL_MAX_SELECTION_SIZE 4

//! @brief A match from a selection evaluation
//!
//! A match is a set of atomic indexes matching a given selection. The size of
/// a match depends on the associated selection, and can vary from 1 to 4.
typedef struct {
    //! The actual size of the match. Elements in `atoms` are significant up
    //! to this value, and filled with `(size_t)-1` for all the other values.
    size_t size;
    //! Indexes matching the associated selection
    size_t atoms[CHFL_MAX_SELECTION_SIZE];
} chfl_match_t;

/*!
* @brief Get the matches for a selection after a call to `chfl_selection_evalutate`
*
* @param selection the selection
* @param matches a pre-allocated array of size (number of matches)
* @param n_matches the number of matches, as indicated by `chfl_selection_evaluate`
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_selection_matches(const CHFL_SELECTION* const selection,
                                               chfl_match_t matches[],
                                               size_t n_matches);

/*!
* @brief Destroy a selection, and free the associated memory
* @param selection The selection to destroy
* @return The status code
*/
CHFL_EXPORT chfl_status chfl_selection_free(CHFL_SELECTION* selection);

#ifdef __cplusplus
}
#endif

#endif
