/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

/*! @file chemharp.h
* Chemharp main C API header.
*
* This file contains all the function definitions for the C API of Chemharp, and
* should be self-documented enough.
*/

#ifndef HARP_CAPI_H
#define HARP_CAPI_H

#define CHEMHARP_PUBLIC
    #include "chemharp/config.hpp"
#undef CHEMHARP_PUBLIC

#ifdef __cplusplus
    #include <cstddef>
extern "C" {

    namespace harp {
        class Trajectory;
        class Frame;
        class Atom;
        class UnitCell;
        class Topology;
    }
    typedef harp::Trajectory CHRP_TRAJECTORY;
    typedef harp::Frame CHRP_FRAME;
    typedef harp::Atom CHRP_ATOM;
    typedef harp::UnitCell CHRP_CELL;
    typedef harp::Topology CHRP_TOPOLOGY;
#else
    #include <stddef.h>
    #include <stdbool.h>
    //! Opaque type handling trajectories files
    typedef struct CHRP_TRAJECTORY CHRP_TRAJECTORY;
    //! Opaque type handling frames, *i.e* data from a step
    typedef struct CHRP_FRAME CHRP_FRAME;
    //! Opaque type handling an atom
    typedef struct CHRP_ATOM CHRP_ATOM;
    //! Opaque type handling an unit cell
    typedef struct CHRP_CELL CHRP_CELL;
    //! Opaque type handling a topology
    typedef struct CHRP_TOPOLOGY CHRP_TOPOLOGY;
#endif

/*!
* @brief Get the error message corresponding to an error code.
* @param status The error code
* @return A null-terminated string encoding the textual representation of the status.
*/
CHRP_EXPORT const char* chrp_strerror(int status);

/*!
* @brief Get the last error message.
* @return A null-terminated string encoding the textual representation of the last error.
*/
CHRP_EXPORT const char* chrp_last_error();

//! Available logging level
typedef enum CHRP_LOG_LEVEL {
    //! Do not log anything
    NONE,
    //! Only log on errors
    ERROR,
    //! Log warnings and erors
    WARNING,
    //! Log infos, warnings and errors
    INFO,
    //! Log everything
    DEBUG
} chrp_log_level_t ;

/*!
* @brief Set the current logging level to \c level
* @param level The new logging level
* @return The status code
*/
CHRP_EXPORT int chrp_loglevel(chrp_log_level_t level);

/*!
* @brief Redirect the logs to \c file, overwriting the file if it exists
* @param file The filename for the new log file.
* @return The status code
*/
CHRP_EXPORT int chrp_logfile(const char* file);

/*!
* @brief Redirect the logs to the standard error output. This is enabled by default.
* @return The status code
*/
CHRP_EXPORT int chrp_log_stderr();

/******************************************************************************/
/*!
* @brief Open a trajectory file.
* @param filename The path to the trajectory file
* @param mode The opening ("r" for read or "w" for write) mode for the file.
* @return A pointer to the file
*/
CHRP_EXPORT CHRP_TRAJECTORY* chrp_open(const char* filename, const char* mode);

/*!
* @brief Read the next step of the trajectory into a frame
* @param file A pointer to the trajectory
* @param frame A frame to fill with the data
* @return The status code.
*/
CHRP_EXPORT int chrp_trajectory_read(CHRP_TRAJECTORY *file, CHRP_FRAME *frame);

/*!
* @brief Read a specific step of the trajectory in a frame
* @param file A pointer to the trajectory
* @param step The step to read
* @param frame A frame to fill with the data
* @return The status code.
*/
CHRP_EXPORT int chrp_trajectory_read_step(CHRP_TRAJECTORY *file, size_t step, CHRP_FRAME* frame);

/*!
* @brief Write a frame to the trajectory.
* @param file The trajectory to write
* @param frame the frame which will be writen to the file
* @return The status code.
*/
CHRP_EXPORT int chrp_trajectory_write(CHRP_TRAJECTORY *file, const CHRP_FRAME *frame);

/*!
* @brief Set the topology associated with a trajectory. This topology will be
*        used when reading and writing the files, replacing any topology in the
*        frames or files.
* @param file A pointer to the trajectory
* @param topology The new topology to use
* @return The status code.
*/
CHRP_EXPORT int chrp_trajectory_set_topology(CHRP_TRAJECTORY *file, const CHRP_TOPOLOGY *topology);

/*!
* @brief Set the topology associated with a trajectory by reading the first
*        frame of \c filename; and extracting the topology of this frame.
* @param file A pointer to the trajectory
* @param filename The file to read in order to get the new topology
* @return The status code.
*/
CHRP_EXPORT int chrp_trajectory_set_topology_file(CHRP_TRAJECTORY *file, const char* filename);

/*!
* @brief Set the unit cell associated with a trajectory. This cell will be
*        used when reading and writing the files, replacing any unit cell in the
*        frames or files.
* @param file A pointer to the trajectory
* @param cell The new cell to use
* @return The status code.
*/
CHRP_EXPORT int chrp_trajectory_set_cell(CHRP_TRAJECTORY *file, const CHRP_CELL *cell);

/*!
* @brief Get the number of steps (the number of frames) in a trajectory.
* @param file A pointer to the trajectory
* @param nsteps This will contain the number of steps
* @return The status code.
*/
CHRP_EXPORT int chrp_trajectory_nsteps(CHRP_TRAJECTORY *file, size_t *nsteps);

/*!
* @brief Close a trajectory file, flush any buffer content to the hard drive, and
*        free the associated memory
* @param file A pointer to the file
* @return The status code
*/
CHRP_EXPORT int chrp_trajectory_close(CHRP_TRAJECTORY *file);

/******************************************************************************/
/*!
* @brief Create an empty frame with initial capacity of \c natoms. It will be
*        resized by the library as needed.
* @param natoms the size of the wanted frame
* @return A pointer to the frame
*/
CHRP_EXPORT CHRP_FRAME* chrp_frame(size_t natoms);

/*!
* @brief Get the frame size, i.e. the current number of atoms
* @param frame The frame to analyse
* @param natoms the number of atoms in the frame
* @return The status code
*/
CHRP_EXPORT int chrp_frame_size(const CHRP_FRAME* frame, size_t *natoms);

/*!
* @brief Get the positions from a frame
* @param frame The frame
* @param data A Nx3 float array to be filled with the data
* @param size The array size (N).
* @return The status code
*/
CHRP_EXPORT int chrp_frame_positions(const CHRP_FRAME* frame, float (*data)[3], size_t size);

/*!
* @brief Set the positions of a frame
* @param frame The frame
* @param data A Nx3 float array containing the positions in row-major order.
* @param size The array size (N).
* @return The status code
*/
CHRP_EXPORT int chrp_frame_set_positions(CHRP_FRAME* frame, float (*data)[3], size_t size);

/*!
* @brief Get the velocities from a frame, if they exists
* @param frame The frame
* @param data A Nx3 float array to be filled with the data
* @param size The array size (N).
* @return The status code
*/
CHRP_EXPORT int chrp_frame_velocities(const CHRP_FRAME* frame, float (*data)[3], size_t size);

/*!
* @brief Set the velocities of a frame.
* @param frame The frame
* @param data A Nx3 float array containing the velocities in row-major order.
* @param size The array size (N).
* @return The status code
*/
CHRP_EXPORT int chrp_frame_set_velocities(CHRP_FRAME* frame, float (*data)[3], size_t size);

/*!
* @brief Check if a frame has velocity information.
* @param frame The frame
* @param has_vel true if the frame has velocities, false otherwise.
* @return The status code
*/
CHRP_EXPORT int chrp_frame_has_velocities(const CHRP_FRAME* frame, bool *has_vel);

/*!
* @brief Set the UnitCell of a Frame.
* @param frame The frame
* @param cell The new cell
* @return The status code
*/
CHRP_EXPORT int chrp_frame_set_cell(CHRP_FRAME* frame, const CHRP_CELL* cell);

/*!
* @brief Set the Topology of a Frame.
* @param frame The frame
* @param topology The new topology
* @return The status code
*/
CHRP_EXPORT int chrp_frame_set_topology(CHRP_FRAME* frame, const CHRP_TOPOLOGY* topology);

/*!
* @brief Get the Frame step, i.e. the frame number in the trajectory
* @param frame The frame
* @param step This will contains the step number
* @return The status code
*/
CHRP_EXPORT int chrp_frame_step(const CHRP_FRAME* frame, size_t* step);

/*!
* @brief Set the Frame step.
* @param frame The frame
* @param step The new frame step
* @return The status code
*/
CHRP_EXPORT int chrp_frame_set_step(CHRP_FRAME* frame, size_t step);

/*!
* @brief Try to guess the bonds, angles and dihedrals in the system. If \c bonds
*        is true, guess everything; else only guess the angles and dihedrals from
*        the topology bond list.
* @param frame The Frame to analyse
* @param bonds Should we recompute the bonds from the positions or not ?
* @return The status code
*/
CHRP_EXPORT int chrp_frame_guess_topology(CHRP_FRAME* frame, bool bonds);

/*!
* @brief Destroy a frame, and free the associated memory
* @param frame The frame to destroy
* @return The status code
*/
CHRP_EXPORT int chrp_frame_free(CHRP_FRAME* frame);

/******************************************************************************/
/*!
* @brief Create an ORTHOROMBIC UnitCell from the three lenghts
* @param a,b,c the three lenghts of the cell
* @return A pointer to the UnitCell
*/
CHRP_EXPORT CHRP_CELL* chrp_cell(double a, double b, double c);

/*!
* @brief Create a TRICLINIC UnitCell from the three lenghts and the three angles
* @param a,b,c the three lenghts of the cell
* @param alpha,beta,gamma the three angles of the cell
* @return A pointer to the UnitCell
*/
CHRP_EXPORT CHRP_CELL* chrp_cell_triclinic(double a, double b, double c, double alpha, double beta, double gamma);

/*!
* @brief Get the UnitCell from a frame
* @param frame the frame
* @return A pointer to the UnitCell
*/
CHRP_EXPORT CHRP_CELL* chrp_cell_from_frame(CHRP_FRAME* frame);

/*!
* @brief Get the cell volume.
* @param cell the unit cell to read
* @param V the volume
* @return The status code
*/
CHRP_EXPORT int chrp_cell_volume(const CHRP_CELL* cell, double* V);

/*!
* @brief Get the cell lenghts.
* @param cell the unit cell to read
* @param a,b,c the three cell lenghts
* @return The status code
*/
CHRP_EXPORT int chrp_cell_lengths(const CHRP_CELL* cell, double* a, double* b, double* c);

/*!
* @brief Set the unit cell lenghts.
* @param cell the unit cell to modify
* @param a,b,c the cell lenghts
* @return The status code
*/
CHRP_EXPORT int chrp_cell_set_lengths(CHRP_CELL* cell, double a, double b, double c);

/*!
* @brief Get the cell angles, in degrees.
* @param cell the cell to read
* @param alpha,beta,gamma the three cell angles
* @return The status code
*/
CHRP_EXPORT int chrp_cell_angles(const CHRP_CELL* cell, double* alpha, double* beta, double* gamma);

/*!
* @brief Set the cell angles, in degrees
* @param cell the unit cell to modify
* @param alpha,beta,gamma the new angles values, in degrees
* @return The status code
*/
CHRP_EXPORT int chrp_cell_set_angles(CHRP_CELL* cell, double alpha, double beta, double gamma);

/*!
* @brief Get the unit cell matricial representation.
* @param cell the unit cell to use
* @param mat the matrix to fill. It should be a 3x3 matrix.
* @return The status code
*/
CHRP_EXPORT int chrp_cell_matrix(const CHRP_CELL* cell, double mat[3][3]);

//! Available cell types in Chemharp
typedef enum CHRP_CELL_TYPES {
    //! The three angles are 90°
    ORTHOROMBIC,
    //! The three angles may not be 90°
    TRICLINIC,
    //! Cell type when there is no periodic boundary conditions
    INFINITE,
} chrp_cell_type_t ;

/*!
* @brief Get the cell type
* @param cell the unit cell to read
* @param type the type of the cell
* @return The status code
*/
CHRP_EXPORT int chrp_cell_type(const CHRP_CELL* cell, chrp_cell_type_t* type);

/*!
* @brief Set the cell type
* @param cell the cell to modify
* @param type the new type of the cell
* @return The status code
*/
CHRP_EXPORT int chrp_cell_set_type(CHRP_CELL* cell, chrp_cell_type_t type);

/*!
* @brief Get the cell periodic boundary conditions along the three axis
* @param cell the cell to read
* @param x,y,z the periodicity of the cell along the three axis.
* @return The status code
*/
CHRP_EXPORT int chrp_cell_periodicity(const CHRP_CELL* cell, bool* x, bool* y, bool* z);

/*!
* @brief Set the cell periodic boundary conditions along the three axis
* @param cell the cell to modify
* @param x,y,z the new periodicity of the cell along the three axis.
* @return The status code
*/
CHRP_EXPORT int chrp_cell_set_periodicity(CHRP_CELL* cell, bool x, bool y, bool z);

/*!
* @brief Destroy an unit cell, and free the associated memory
* @param cell The cell to destroy
* @return The status code
*/
CHRP_EXPORT int chrp_cell_free(CHRP_CELL* cell);

/******************************************************************************/

/*!
* @brief Create a new empty topology
* @return A pointer to the new Topology
*/
CHRP_EXPORT CHRP_TOPOLOGY* chrp_topology();

/*!
* @brief Extract the topology from a frame
* @param frame The frame
* @return A pointer to the new Topology
*/
CHRP_EXPORT CHRP_TOPOLOGY* chrp_topology_from_frame(CHRP_FRAME* frame);

/*!
* @brief Get the topology size, i.e. the current number of atoms
* @param topology The topology to analyse
* @param natoms Will contain the number of atoms in the frame
* @return The status code
*/
CHRP_EXPORT int chrp_topology_size(const CHRP_TOPOLOGY* topology, size_t *natoms);

/*!
* @brief Add an atom to a topology
* @param topology The topology
* @param atom The atom to be added
* @return The status code
*/
CHRP_EXPORT int chrp_topology_append(CHRP_TOPOLOGY* topology, const CHRP_ATOM* atom);

/*!
* @brief Remove an atom from a topology by index
* @param topology The topology
* @param i The atomic index
* @return The status code
*/
CHRP_EXPORT int chrp_topology_remove(CHRP_TOPOLOGY* topology, size_t i);

/*!
* @brief Tell if the atoms \c i and \c j are bonded
* @param topology The topology
* @param i,j The atomic indexes
* @param result true if the atoms are bonded, false otherwise
* @return The status code
*/
CHRP_EXPORT int chrp_topology_isbond(CHRP_TOPOLOGY* topology, size_t i, size_t j, bool* result);

/*!
* @brief Tell if the atoms \c i, \c j and \c k constitues an angle
* @param topology The topology
* @param i,j,k The atomic indexes
* @param result true if the atoms constitues an angle, false otherwise
* @return The status code
*/
CHRP_EXPORT int chrp_topology_isangle(CHRP_TOPOLOGY* topology,
                            size_t i, size_t j, size_t k, bool* result);

/*!
* @brief Tell if the atoms \c i, \c j, \c k and \c m constitues a dihedral angle
* @param topology The topology
* @param i,j,k,m The atomic indexes
* @param result true if the atoms constitues a dihedral angle, false otherwise
* @return The status code
*/
CHRP_EXPORT int chrp_topology_isdihedral(CHRP_TOPOLOGY* topology,
                             size_t i, size_t j, size_t k, size_t m, bool* result);

/*!
* @brief Get the number of bonds in the system
* @param topology The topology
* @param nbonds After the call, contains the number of bond
* @return The status code
*/
CHRP_EXPORT int chrp_topology_bonds_count(CHRP_TOPOLOGY* topology, size_t* nbonds);

/*!
* @brief Get the number of angles in the system
* @param topology The topology
* @param nangles After the call, contains the number of angles
* @return The status code
*/
CHRP_EXPORT int chrp_topology_angles_count(CHRP_TOPOLOGY* topology, size_t* nangles);

/*!
* @brief Get the number of dihedral angles in the system
* @param topology The topology
* @param ndihedrals After the call, contains the number of dihedral angles
* @return The status code
*/
CHRP_EXPORT int chrp_topology_dihedrals_count(CHRP_TOPOLOGY* topology, size_t* ndihedrals);

/*!
* @brief Get the bonds in the system
* @param topology The topology
* @param data A nbonds x 2 array to be filled with the bonds in the system
* @param nbonds The size of the array. This should equal the value given by the
*               chrp_topology_bonds_count function
* @return The status code
*/
CHRP_EXPORT int chrp_topology_bonds(CHRP_TOPOLOGY* topology, size_t (*data)[2], size_t nbonds);

/*!
* @brief Get the angles in the system
* @param topology The topology
* @param data A nangles x 3 array to be filled with the angles in the system
* @param nangles The size of the array. This should equal the value given by the
*               chrp_topology_angles_count function
* @return The status code
*/
CHRP_EXPORT int chrp_topology_angles(CHRP_TOPOLOGY* topology, size_t (*data)[3], size_t nangles);

/*!
* @brief Get the dihedral angles in the system
* @param topology The topology
* @param data A ndihedrals x 4 array to be filled with the dihedral angles in the system
* @param ndihedrals The size of the array. This should equal the value given by the
*               chrp_topology_dihedrals_count function
* @return The status code
*/
CHRP_EXPORT int chrp_topology_dihedrals(CHRP_TOPOLOGY* topology, size_t (*data)[4], size_t ndihedrals);

/*!
* @brief Add a bond between the atoms \c i and \c j in the system
* @param topology The topology
* @param i,j The atomic indexes
* @return The status code
*/
CHRP_EXPORT int chrp_topology_add_bond(CHRP_TOPOLOGY* topology, size_t i, size_t j);

/*!
* @brief Remove any existing bond between the atoms \c i and \c j in the system
* @param topology The topology
* @param i,j The atomic indexes
* @return The status code
*/
CHRP_EXPORT int chrp_topology_remove_bond(CHRP_TOPOLOGY* topology, size_t i, size_t j);

/*!
* @brief Destroy a topology, and free the associated memory
* @param topology The topology to destroy
* @return The status code
*/
CHRP_EXPORT int chrp_topology_free(CHRP_TOPOLOGY* topology);

/******************************************************************************/

/*!
* @brief Create an atom from an atomic name
* @param name The new atom name
* @return A pointer to the corresponding atom
*/
CHRP_EXPORT CHRP_ATOM* chrp_atom(const char* name);

/*!
* @brief Get a specific atom from a frame
* @param frame The frame
* @param idx The atom index in the frame
* @return A pointer to the corresponding atom
*/
CHRP_EXPORT CHRP_ATOM* chrp_atom_from_frame(CHRP_FRAME* frame, size_t idx);

/*!
* @brief Get a specific atom from a topology
* @param topology The topology
* @param idx The atom index in the topology
* @return A pointer to the corresponding atom
*/
CHRP_EXPORT CHRP_ATOM* chrp_atom_from_topology(CHRP_TOPOLOGY* topology, size_t idx);

/*!
* @brief Get the mass of an atom, in atomic mass units
* @param atom The atom
* @param mass The atom mass
* @return The status code
*/
CHRP_EXPORT int chrp_atom_mass(const CHRP_ATOM* atom, float* mass);

/*!
* @brief Set the mass of an atom, in atomic mass units
* @param atom The atom
* @param mass The new atom mass
* @return The status code
*/
CHRP_EXPORT int chrp_atom_set_mass(CHRP_ATOM* atom, float mass);

/*!
* @brief Get the charge of an atom, in number of the electron charge e
* @param atom The atom
* @param charge The atom charge
* @return The status code
*/
CHRP_EXPORT int chrp_atom_charge(const CHRP_ATOM* atom, float* charge);

/*!
* @brief Set the charge of an atom, in number of the electron charge e
* @param atom The atom
* @param charge The new atom charge
* @return The status code
*/
CHRP_EXPORT int chrp_atom_set_charge(CHRP_ATOM* atom, float charge);

/*!
* @brief Get the name of an atom
* @param atom The atom
* @param name A string buffer to be filled with the name
* @param buffsize The size of the string buffer
* @return The status code
*/
CHRP_EXPORT int chrp_atom_name(const CHRP_ATOM* atom, char* name, size_t buffsize);

/*!
* @brief Set the name of an atom
* @param atom The atom
* @param name A null terminated string containing the new name
* @return The status code
*/
CHRP_EXPORT int chrp_atom_set_name(CHRP_ATOM* atom, const char* name);

/*!
* @brief Try to get the full name of an atom from the short name
* @param atom The atom
* @param name A string buffer to be filled with the name
* @param buffsize The size of the string buffer
* @return The status code
*/
CHRP_EXPORT int chrp_atom_full_name(const CHRP_ATOM* atom, char* name, size_t buffsize);

/*!
* @brief Try to get the Van der Waals radius of an atom from the short name
* @param atom The atom
* @param radius The Van der Waals radius of the atom or -1 if no value could be found.
* @return The status code
*/
CHRP_EXPORT int chrp_atom_vdw_radius(const CHRP_ATOM* atom, double* radius);

/*!
* @brief Try to get the covalent radius of an atom from the short name
* @param atom The atom
* @param radius The covalent radius of the atom or -1 if no value could be found.
* @return The status code
*/
CHRP_EXPORT int chrp_atom_covalent_radius(const CHRP_ATOM* atom, double* radius);

/*!
* @brief Try to get the atomic number of an atom from the short name
* @param atom The atom
* @param number The atomic number, or -1 if no value could be found.
* @return The status code
*/
CHRP_EXPORT int chrp_atom_atomic_number(const CHRP_ATOM* atom, int* number);

/*!
* @brief Destroy an atom, and free the associated memory
* @param atom The atom to destroy
* @return The status code
*/
CHRP_EXPORT int chrp_atom_free(CHRP_ATOM* atom);


#ifdef __cplusplus
}
#endif

#endif
