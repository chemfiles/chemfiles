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
    }
    typedef chemfiles::Trajectory CHFL_TRAJECTORY;
    typedef chemfiles::Frame CHFL_FRAME;
    typedef chemfiles::Atom CHFL_ATOM;
    typedef chemfiles::UnitCell CHFL_CELL;
    typedef chemfiles::Topology CHFL_TOPOLOGY;
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
#endif
// clang-format on

//! Status code for success
#define CHFL_SUCCESS 0
//! Memory error: out of memory, wrong size for arrays parameters, ...
#define CHFL_MEMORY_ERROR 1
//! File error: file do not exist, you do not have rights to open it, ...
#define CHFL_FILE_ERROR 2
//! Error in file formating
#define CHFL_FORMAT_ERROR 3
//! Error in selection parsing
#define CHFL_SELECTION_ERROR 4
//! Any other error from Chemfiles
#define CHFL_GENERIC_ERROR 5
//! Error in the C++ standard library
#define CHFL_CXX_ERROR 6

/*!
* @brief Get the version of the chemfiles library
* @return A null-terminated string containing the version of Chemfiles.
*/
CHFL_EXPORT const char* chfl_version(void);

/*!
* @brief Get the error message corresponding to an error code.
* @param status The error code
* @return A null-terminated string encoding the textual representation of the
*         status.
*/
CHFL_EXPORT const char* chfl_strerror(int status);

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
CHFL_EXPORT int chfl_clear_errors(void);

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
CHFL_EXPORT int chfl_loglevel(chfl_log_level_t* level);

/*!
* @brief Set the current maximal logging level to `level`
* @param level The new logging level
* @return The status code
*/
CHFL_EXPORT int chfl_set_loglevel(chfl_log_level_t level);

/*!
* @brief Redirect the logs to `file`, overwriting the file if it exists
* @param file The filename for the new log file.
* @return The status code
*/
CHFL_EXPORT int chfl_logfile(const char* file);

/*!
* @brief Redirect the logs to the standard output.
* @return The status code
*/
CHFL_EXPORT int chfl_log_stdout(void);

/*!
* @brief Redirect the logs to the standard error output. This is enabled by
*        default.
* @return The status code
*/
CHFL_EXPORT int chfl_log_stderr(void);

/*!
* @brief Remove all logging output
* @return The status code
*/
CHFL_EXPORT int chfl_log_silent(void);

//! Callback function type for user-provided logging
typedef void (*chfl_logging_cb)(chfl_log_level_t level, const char* message);

/*!
* @brief Redirect all logging to user-provided logging. The `callback` function
* will be called at each loggin operation with the level of the message, and
* the message itself.
* @return The status code
*/
CHFL_EXPORT int chfl_log_callback(chfl_logging_cb callback);

/******************************************************************************/
/*!
* @brief Open a trajectory file.
* @param filename The path to the trajectory file
* @param mode The opening mode: 'r' for read, 'w' for write and 'a' for append.
* @return A pointer to the file
*/
CHFL_EXPORT CHFL_TRAJECTORY* chfl_trajectory_open(const char* filename, char mode);

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
* @return A pointer to the file
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
CHFL_EXPORT int chfl_trajectory_read(CHFL_TRAJECTORY* file, CHFL_FRAME* frame);

/*!
* @brief Read a specific step of the trajectory in a frame
* @param file A pointer to the trajectory
* @param step The step to read
* @param frame A frame to fill with the data
* @return The status code.
*/
CHFL_EXPORT int chfl_trajectory_read_step(CHFL_TRAJECTORY* file,
                                          size_t step,
                                          CHFL_FRAME* frame);

/*!
* @brief Write a frame to the trajectory.
* @param file The trajectory to write
* @param frame the frame which will be writen to the file
* @return The status code.
*/
CHFL_EXPORT int chfl_trajectory_write(CHFL_TRAJECTORY* file,
                                      const CHFL_FRAME* frame);

/*!
* @brief Set the topology associated with a trajectory. This topology will be
*        used when reading and writing the files, replacing any topology in the
*        frames or files.
* @param file A pointer to the trajectory
* @param topology The new topology to use
* @return The status code.
*/
CHFL_EXPORT int chfl_trajectory_set_topology(CHFL_TRAJECTORY* file,
                                             const CHFL_TOPOLOGY* topology);

/*!
* @brief Set the topology associated with a trajectory by reading the first
*        frame of `filename`; and extracting the topology of this frame.
* @param file A pointer to the trajectory
* @param filename The file to read in order to get the new topology
* @return The status code.
*/
CHFL_EXPORT int chfl_trajectory_set_topology_file(CHFL_TRAJECTORY* file,
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
CHFL_EXPORT int chfl_trajectory_set_topology_with_format(CHFL_TRAJECTORY* file,
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
CHFL_EXPORT int chfl_trajectory_set_cell(CHFL_TRAJECTORY* file,
                                         const CHFL_CELL* cell);

/*!
* @brief Get the number of steps (the number of frames) in a trajectory.
* @param file A pointer to the trajectory
* @param nsteps This will contain the number of steps
* @return The status code.
*/
CHFL_EXPORT int chfl_trajectory_nsteps(CHFL_TRAJECTORY* file, size_t* nsteps);

/*!
* @brief Synchronize any buffered content to the hard drive.
* @param file A pointer to the file
* @return The status code
*/
CHFL_EXPORT int chfl_trajectory_sync(CHFL_TRAJECTORY* file);

/*!
* @brief Close a trajectory file, and free the associated memory
* @param file A pointer to the file
* @return The status code
*/
CHFL_EXPORT int chfl_trajectory_close(CHFL_TRAJECTORY* file);

/******************************************************************************/
/*!
* @brief Create an empty frame with initial capacity of `natoms`. It will be
*        resized by the library as needed.
* @param natoms the size of the wanted frame
* @return A pointer to the frame
*/
CHFL_EXPORT CHFL_FRAME* chfl_frame(size_t natoms);

/*!
* @brief Get the current number of atoms in the frame.
* @param frame The frame to analyse
* @param natoms the number of atoms in the frame
* @return The status code
*/
CHFL_EXPORT int chfl_frame_atoms_count(const CHFL_FRAME* frame, size_t* natoms);

/*!
* @brief Get a pointer to the positions array from a frame.
*
* The positions are stored as a N x 3 array, this function set a pointer to
* point to the first element of this array, and give the value of N. If the
* frame is resized (by writing to it, or calling `chfl_frame_resize`), the
* pointer is invalidated.
*
* @param frame The frame
* @param data A pointer to a pointer to float[3] array, which will point to the
*             data
* @param size A pointer to the an integer to be filled with the array size
* @return The status code
*/
CHFL_EXPORT int
chfl_frame_positions(CHFL_FRAME* frame, float (**data)[3], size_t* size);

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
* @param data A pointer to a pointer to float[3] array, which will point to the
*             data
* @param size A pointer to the an integer to be filled with the array size
* @return The status code
*/
CHFL_EXPORT int
chfl_frame_velocities(CHFL_FRAME* frame, float (**data)[3], size_t* size);

/*!
* @brief Resize the positions and the velocities in frame, to make space for N
*        atoms.
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
CHFL_EXPORT int chfl_frame_resize(CHFL_FRAME* frame, size_t natoms);

/*!
* @brief Add velocity storage to this frame.
*
* The storage is initialized with the result of `chfl_frame_atoms_count` as
* number of atoms. If the frame already have velocities, this does nothing.
*
* @param frame The frame
* @return The status code
*/
CHFL_EXPORT int chfl_frame_add_velocities(CHFL_FRAME* frame);

/*!
* @brief Ask wether this frame contains velocity data or not.
* @param frame The frame
* @param has_velocities A boolean, will be true if the frame have velocities,
*                       false otherwise.
* @return The status code
*/
CHFL_EXPORT int chfl_frame_has_velocities(const CHFL_FRAME* frame,
                                          bool* has_velocities);

/*!
* @brief Set the UnitCell of a Frame.
* @param frame The frame
* @param cell The new cell
* @return The status code
*/
CHFL_EXPORT int chfl_frame_set_cell(CHFL_FRAME* frame, const CHFL_CELL* cell);

/*!
* @brief Set the Topology of a Frame.
* @param frame The frame
* @param topology The new topology
* @return The status code
*/
CHFL_EXPORT int chfl_frame_set_topology(CHFL_FRAME* frame,
                                        const CHFL_TOPOLOGY* topology);

/*!
* @brief Get the Frame step, i.e. the frame number in the trajectory
* @param frame The frame
* @param step This will contains the step number
* @return The status code
*/
CHFL_EXPORT int chfl_frame_step(const CHFL_FRAME* frame, size_t* step);

/*!
* @brief Set the Frame step.
* @param frame The frame
* @param step The new frame step
* @return The status code
*/
CHFL_EXPORT int chfl_frame_set_step(CHFL_FRAME* frame, size_t step);

/*!
* @brief  Guess the bonds, angles and dihedrals in the system.
*
* The bonds are guessed using a distance-based algorithm, and then angles and
* dihedrals are guessed from the bonds.
*
* @param frame The Frame to analyse
* @return The status code
*/
CHFL_EXPORT int chfl_frame_guess_topology(CHFL_FRAME* frame);

/*!
* @brief Select atoms in a frame, from a specific selection string.
*
* This function select atoms in a frame matching a selection string. For
* example, "name H and x > 4" will select all the atoms with name "H" and
* $x$ coordinate less than 4. See the C++ documentation for the full selection
* language.
*
* Results of this function are used to fill a pre-allocated array containing
* `natoms` bool, where `natoms` is the number of atoms in the frame. The array
* will contain `true` at position `i` if the atom at position `i` matches the
* selection string, and false otherwise.
*
* @pre The range from `matched` to `(matched + natoms)` is a valid adress range.
*
* @param frame The frame to analyse
* @param selection A null-terminated string containing the selection string
* @param matched a pre-allocated array, with space for `natoms` booleans
* @param natoms the size of the `matched` array. This MUST be the same number as
*               the `frame` number of atoms.
* @return The status code
*/
CHFL_EXPORT int chfl_frame_selection(const CHFL_FRAME* frame,
                                     const char* selection,
                                     bool matched[],
                                     size_t natoms);

/*!
* @brief Destroy a frame, and free the associated memory
* @param frame The frame to destroy
* @return The status code
*/
CHFL_EXPORT int chfl_frame_free(CHFL_FRAME* frame);

/******************************************************************************/
/*!
* @brief Create an ORTHORHOMBIC UnitCell from the three lenghts
* @param a first lenght of the cell (in Angstroms)
* @param b second lenght of the cell (in Angstroms)
* @param c third lenght of the cell (in Angstroms)
* @return A pointer to the UnitCell
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
* @return A pointer to the UnitCell
*/
CHFL_EXPORT CHFL_CELL* chfl_cell_triclinic(
    double a, double b, double c, double alpha, double beta, double gamma);

/*!
* @brief Get the UnitCell from a frame
* @param frame the frame
* @return A pointer to the UnitCell
*/
CHFL_EXPORT CHFL_CELL* chfl_cell_from_frame(const CHFL_FRAME* frame);

/*!
* @brief Get the cell volume.
* @param cell the unit cell to read
* @param V the volume
* @return The status code
*/
CHFL_EXPORT int chfl_cell_volume(const CHFL_CELL* cell, double* V);

/*!
* @brief Get the cell lenghts.
* @param cell the unit cell to read
* @param a first lenght of the cell (in Angstroms)
* @param b second lenght of the cell (in Angstroms)
* @param c third lenght of the cell (in Angstroms)
* @return The status code
*/
CHFL_EXPORT int
chfl_cell_lengths(const CHFL_CELL* cell, double* a, double* b, double* c);

/*!
* @brief Set the unit cell lenghts.
* @param cell the unit cell to modify
* @param a first lenght of the cell (in Angstroms)
* @param b second lenght of the cell (in Angstroms)
* @param c third lenght of the cell (in Angstroms)
* @return The status code
*/
CHFL_EXPORT int
chfl_cell_set_lengths(CHFL_CELL* cell, double a, double b, double c);

/*!
* @brief Get the cell angles, in degrees.
* @param cell the cell to read
* @param alpha angle of the cell between the vectors `b` and `c` (in degree)
* @param beta angle of the cell between the vectors `a` and `c` (in degree)
* @param gamma angle of the cell between the vectors `a` and `b` (in degree)
* @return The status code
*/
CHFL_EXPORT int chfl_cell_angles(const CHFL_CELL* cell,
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
CHFL_EXPORT int
chfl_cell_set_angles(CHFL_CELL* cell, double alpha, double beta, double gamma);

/*!
* @brief Get the unit cell matricial representation.
* @param cell the unit cell to use
* @param matrix the matrix to fill. It should be a 3x3 matrix.
* @return The status code
*/
CHFL_EXPORT int chfl_cell_matrix(const CHFL_CELL* cell, double matrix[3][3]);

//! Available cell types in chemfiles
typedef enum CHFL_CELL_TYPES {
    //! The three angles are 90°
    CHFL_CELL_ORTHORHOMBIC = 0,
    //! The three angles may not be 90°
    CHFL_CELL_TRICLINIC = 1,
    //! Cell type when there is no periodic boundary conditions
    CHFL_CELL_INFINITE = 2,
} chfl_cell_type_t;

/*!
* @brief Get the cell type
* @param cell the unit cell to read
* @param type the type of the cell
* @return The status code
*/
CHFL_EXPORT int chfl_cell_type(const CHFL_CELL* cell, chfl_cell_type_t* type);

/*!
* @brief Set the cell type
* @param cell the cell to modify
* @param type the new type of the cell
* @return The status code
*/
CHFL_EXPORT int chfl_cell_set_type(CHFL_CELL* cell, chfl_cell_type_t type);

/*!
* @brief Destroy an unit cell, and free the associated memory
* @param cell The cell to destroy
* @return The status code
*/
CHFL_EXPORT int chfl_cell_free(CHFL_CELL* cell);

/******************************************************************************/

/*!
* @brief Create a new empty topology
* @return A pointer to the new Topology
*/
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology();

/*!
* @brief Get a copy of the topology of a frame
* @param frame The frame
* @return A pointer to the new Topology
*/
CHFL_EXPORT CHFL_TOPOLOGY* chfl_topology_from_frame(const CHFL_FRAME* frame);

/*!
* @brief Get the current number of atoms in the topology.
* @param topology The topology to analyse
* @param natoms Will contain the number of atoms in the frame
* @return The status code
*/
CHFL_EXPORT int chfl_topology_atoms_count(const CHFL_TOPOLOGY* topology,
                                          size_t* natoms);

/*!
* @brief Add an atom at the end of a topology
* @param topology The topology
* @param atom The atom to be added
* @return The status code
*/
CHFL_EXPORT int chfl_topology_append(CHFL_TOPOLOGY* topology,
                                     const CHFL_ATOM* atom);

/*!
* @brief Remove an atom from a topology by index. This modify all the other
* atoms indexes.
* @param topology The topology
* @param i The atomic index
* @return The status code
*/
CHFL_EXPORT int chfl_topology_remove(CHFL_TOPOLOGY* topology, size_t i);

/*!
* @brief Tell if the atoms `i` and `j` are bonded together
* @param topology The topology
* @param i index of the first atom in the topology
* @param j index of the second atom in the topology
* @param result true if the atoms are bonded, false otherwise
* @return The status code
*/
CHFL_EXPORT int chfl_topology_isbond(const CHFL_TOPOLOGY* topology,
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
CHFL_EXPORT int chfl_topology_isangle(
    const CHFL_TOPOLOGY* topology, size_t i, size_t j, size_t k, bool* result);

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
CHFL_EXPORT int chfl_topology_isdihedral(const CHFL_TOPOLOGY* topology,
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
CHFL_EXPORT int chfl_topology_bonds_count(const CHFL_TOPOLOGY* topology,
                                          size_t* nbonds);

/*!
* @brief Get the number of angles in the system
* @param topology The topology
* @param nangles After the call, contains the number of angles
* @return The status code
*/
CHFL_EXPORT int chfl_topology_angles_count(const CHFL_TOPOLOGY* topology,
                                           size_t* nangles);

/*!
* @brief Get the number of dihedral angles in the system
* @param topology The topology
* @param ndihedrals After the call, contains the number of dihedral angles
* @return The status code
*/
CHFL_EXPORT int chfl_topology_dihedrals_count(const CHFL_TOPOLOGY* topology,
                                              size_t* ndihedrals);

/*!
* @brief Get the list of bonds in the system
* @param topology The topology
* @param data A nbonds x 2 array to be filled with the bonds in the system
* @param nbonds The size of the array. This should equal the value given by the
*               chfl_topology_bonds_count function
* @return The status code
*/
CHFL_EXPORT int chfl_topology_bonds(const CHFL_TOPOLOGY* topology,
                                    size_t (*data)[2],
                                    size_t nbonds);

/*!
* @brief Get the list of angles in the system
* @param topology The topology
* @param data A nangles x 3 array to be filled with the angles in the system
* @param nangles The size of the array. This should equal the value given by the
*               chfl_topology_angles_count function
* @return The status code
*/
CHFL_EXPORT int chfl_topology_angles(const CHFL_TOPOLOGY* topology,
                                     size_t (*data)[3],
                                     size_t nangles);

/*!
* @brief Get the list of dihedral angles in the system
* @param topology The topology
* @param data A ndihedrals x 4 array to be filled with the dihedral angles in
* the system
* @param ndihedrals The size of the array. This should equal the value given by
* the
*               chfl_topology_dihedrals_count function
* @return The status code
*/
CHFL_EXPORT int chfl_topology_dihedrals(const CHFL_TOPOLOGY* topology,
                                        size_t (*data)[4],
                                        size_t ndihedrals);

/*!
* @brief Add a bond between the atoms `i` and `j` in the system
* @param topology The topology
* @param i index of the first atom in the topology
* @param j index of the second atom in the topology
* @return The status code
*/
CHFL_EXPORT int
chfl_topology_add_bond(CHFL_TOPOLOGY* topology, size_t i, size_t j);

/*!
* @brief Remove any existing bond between the atoms `i` and `j` in the system
* @param topology The topology
* @param i index of the first atom in the topology
* @param j index of the second atom in the topology
* @return The status code
*/
CHFL_EXPORT int
chfl_topology_remove_bond(CHFL_TOPOLOGY* topology, size_t i, size_t j);

/*!
* @brief Destroy a topology, and free the associated memory
* @param topology The topology to destroy
* @return The status code
*/
CHFL_EXPORT int chfl_topology_free(CHFL_TOPOLOGY* topology);

/******************************************************************************/

/*!
* @brief Create an atom from an atomic name
* @param name The new atom name
* @return A pointer to the corresponding atom
*/
CHFL_EXPORT CHFL_ATOM* chfl_atom(const char* name);

/*!
* @brief Get a specific atom from a frame
* @param frame The frame
* @param idx The atom index in the frame
* @return A pointer to the corresponding atom
*/
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_frame(const CHFL_FRAME* frame,
                                            size_t idx);

/*!
* @brief Get a specific atom from a topology
* @param topology The topology
* @param idx The atom index in the topology
* @return A pointer to the corresponding atom
*/
CHFL_EXPORT CHFL_ATOM* chfl_atom_from_topology(const CHFL_TOPOLOGY* topology,
                                               size_t idx);

/*!
* @brief Get the mass of an atom, in atomic mass units
* @param atom The atom
* @param mass The atom mass
* @return The status code
*/
CHFL_EXPORT int chfl_atom_mass(const CHFL_ATOM* atom, float* mass);

/*!
* @brief Set the mass of an atom, in atomic mass units
* @param atom The atom
* @param mass The new atom mass
* @return The status code
*/
CHFL_EXPORT int chfl_atom_set_mass(CHFL_ATOM* atom, float mass);

/*!
* @brief Get the charge of an atom, in number of the electron charge e
* @param atom The atom
* @param charge The atom charge
* @return The status code
*/
CHFL_EXPORT int chfl_atom_charge(const CHFL_ATOM* atom, float* charge);

/*!
* @brief Set the charge of an atom, in number of the electron charge e
* @param atom The atom
* @param charge The new atom charge
* @return The status code
*/
CHFL_EXPORT int chfl_atom_set_charge(CHFL_ATOM* atom, float charge);

/*!
* @brief Get the name of an atom
* @param atom The atom
* @param name A string buffer to be filled with the name
* @param buffsize The size of the string buffer
* @return The status code
*/
CHFL_EXPORT int
chfl_atom_name(const CHFL_ATOM* atom, char* name, size_t buffsize);

/*!
* @brief Set the name of an atom
* @param atom The atom
* @param name A null terminated string containing the new name
* @return The status code
*/
CHFL_EXPORT int chfl_atom_set_name(CHFL_ATOM* atom, const char* name);

/*!
* @brief Try to get the full name of an atom from the short name
* @param atom The atom
* @param name A string buffer to be filled with the name
* @param buffsize The size of the string buffer
* @return The status code
*/
CHFL_EXPORT int
chfl_atom_full_name(const CHFL_ATOM* atom, char* name, size_t buffsize);

/*!
* @brief Try to get the Van der Waals radius of an atom from the short name
* @param atom The atom
* @param radius The Van der Waals radius of the atom or -1 if no value could be
* found.
* @return The status code
*/
CHFL_EXPORT int chfl_atom_vdw_radius(const CHFL_ATOM* atom, double* radius);

/*!
* @brief Try to get the covalent radius of an atom from the short name
* @param atom The atom
* @param radius The covalent radius of the atom or -1 if no value could be
* found.
* @return The status code
*/
CHFL_EXPORT int chfl_atom_covalent_radius(const CHFL_ATOM* atom,
                                          double* radius);

/*!
* @brief Try to get the atomic number of an atom from the short name
* @param atom The atom
* @param number The atomic number, or -1 if no value could be found.
* @return The status code
*/
CHFL_EXPORT int chfl_atom_atomic_number(const CHFL_ATOM* atom, int* number);

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
CHFL_EXPORT int chfl_atom_type(const CHFL_ATOM* atom, chfl_atom_type_t* type);

/*!
* @brief Set the atom type
* @param atom the atom to modify
* @param type the new type of the atom
* @return The status code
*/
CHFL_EXPORT int chfl_atom_set_type(CHFL_ATOM* atom, chfl_atom_type_t type);

/*!
* @brief Destroy an atom, and free the associated memory
* @param atom The atom to destroy
* @return The status code
*/
CHFL_EXPORT int chfl_atom_free(CHFL_ATOM* atom);

#ifdef __cplusplus
}
#endif

#endif
