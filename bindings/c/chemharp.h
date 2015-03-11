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
    #include <stdint.h>
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
* @return A textual representation of the error.
*/
const char* chrp_strerror(int status);

/*!
* @brief Get the last error message.
* @return A textual representation of the error.
*/
const char* chrp_last_error(void);

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
* @brief Set the current log level to \c level
*/
void chrp_loglevel(chrp_log_level_t level);

/*!
* @brief Redirect the logs to \c file, overwriting the file if it exists
*/
void chrp_logfile(const char* file);

/*!
* @brief Redirect the logs to the standard error output. This is enabled by default.
*/
void chrp_log_stderr(void);

/******************************************************************************/
/*!
* @brief Open a trajectory file.
* @param filename The filename
* @param mode The opening ("r" or "w") for the file. The default is read mode.
* @return A pointer to the file
*/
CHRP_TRAJECTORY* chrp_open(const char* filename, const char* mode);

/*!
* @brief Read a specific step of a trajectory in a frame
* @param file A pointer to the file
* @param step The step to read
* @param frame A frame to fill with the data
* @return The status code.
*/
int chrp_read_step(CHRP_TRAJECTORY *file, size_t step, CHRP_FRAME* frame);

/*!
* @brief Read the next step of a trajectory in a frame
* @param file A pointer to the file
* @param frame A frame to fill with the data
* @return The status code.
*/
int chrp_read_next_step(CHRP_TRAJECTORY *file, CHRP_FRAME *frame);

/*!
* @brief Write a step (a frame) to a trajectory.
* @param file The file to write
* @param frame the frame which will be writen to the file
* @return The status code.
*/
int chrp_write_step(CHRP_TRAJECTORY *file, CHRP_FRAME *frame);

/*!
* @brief Close a trajectory file, and free the associated memory
* @param file A pointer to the file
* @return The status code
*/
int chrp_close(CHRP_TRAJECTORY *file);

/******************************************************************************/
/*!
* @brief Create an empty frame with initial capacity of \c natoms. It will be
*        resized if needed.
* @param natoms the size of the wanted frame
* @return A pointer to the frame
*/
CHRP_FRAME* chrp_frame(size_t natoms);

/*!
* @brief Get the frame size, i.e. the current number of atoms
* @param frame The frame to analyse
* @return natoms the number of atoms in the frame
* @return The status code
*/
int chrp_frame_size(const CHRP_FRAME* frame, size_t *natoms);

/*!
* @brief Get the positions from a frame
* @param frame The frame
* @param data A Nx3 float array to be filled with the data
* @param size The array size (N).
* @return The status code
*/
int chrp_frame_positions(const CHRP_FRAME* frame, float** data, size_t size);

/*!
* @brief Set the positions from a frame
* @param frame The frame
* @param data A Nx3 float array containing the positions in row-major order.
* @param size The array size (N).
* @return The status code
*/
int chrp_frame_positions_set(CHRP_FRAME* frame, float** data, size_t size);

/*!
* @brief Get the velocities from a frame, if they exists
* @param frame The frame
* @param data A Nx3 float array to be filled with the data
* @param size The array size (N).
* @return The status code
*/
int chrp_frame_velocities(const CHRP_FRAME* frame, float** data, size_t size);

/*!
* @brief Set the velocities from a frame.
* @param frame The frame
* @param data A Nx3 float array containing the velocities in row-major order.
* @param size The array size (N).
* @return The status code
*/
int chrp_frame_velocities_set(CHRP_FRAME* frame, float** data, size_t size);

/*!
* @brief Destroy a frame, and free the associated memory
* @param frame The frame to destroy
* @return The status code
*/
int chrp_frame_free(CHRP_FRAME* frame);

/******************************************************************************/
/*!
* @brief Get the topology from a frame
* @param frame the frame
* @return A pointer to the topology
*/
CHRP_CELL* chrp_cell(CHRP_FRAME* frame);

/*!
* @brief Get the cell lenghts.
* @param cell the unit cell to read
* @param a,b,c the three cell lenghts
* @return The status code
*/
int chrp_cell_lengths(const CHRP_CELL* cell, double* a, double* b, double* c);

/*!
* @brief Set the unit cell lenghts.
* @param cell the unit cell to modify
* @param a,b,c the cell lenghts
* @return The status code
*/
int chrp_cell_lengths_set(CHRP_CELL* cell, double a, double b, double c);

/*!
* @brief Get the cell angles, in degrees.
* @param cell the cell to read
* @param alpha,beta,gamma the three cell angles
* @return The status code
*/
int chrp_cell_angles(const CHRP_CELL* cell, double* alpha, double* beta, double* gamma);

/*!
* @brief Set the cell angles, in degrees
* @param cell the unit cell to modify
* @param alpha,beta,gamma the angles to use, in degrees
* @return The status code
*/
int chrp_cell_angles_set(CHRP_CELL* cell, double alpha, double beta, double gamma);

/*!
* @brief Get the unit cell matricial representation.
* @param cell the unit cell to use
* @param mat the matrix to fill. It should be a 3x3 matrix.
* @return The status code
*/
int chrp_cell_matrix(const CHRP_CELL* cell, double* mat[]);

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
int chrp_cell_type(const CHRP_CELL* cell, chrp_cell_type_t* type);

/*!
* @brief Set the cell type
* @param cell the cell to modify
* @param type the type of the cell
* @return The status code
*/
int chrp_cell_type_set(CHRP_CELL* cell, chrp_cell_type_t type);

/*!
* @brief Get the cell periodic boundary conditions along the three axis
* @param cell the cell to read
* @param x,y,z the periodicity of the cell along the three axis.
* @return The status code
*/
int chrp_cell_periodicity(const CHRP_CELL* cell, bool* x, bool* y, bool* z);

/*!
* @brief Set the cell periodic boundary conditions along the three axis
* @param cell the cell to modify
* @return The status code
*/
int chrp_cell_periodicity_set(CHRP_CELL* cell, bool x, bool y, bool z);

/*!
* @brief Destroy an unit cell, and free the associated memory
* @param cell The cell to destroy
* @return The status code
*/
int chrp_cell_free(CHRP_CELL* cell);

/******************************************************************************/
// TODO CHRP_TOPOLOGY

/******************************************************************************/
/*!
* @brief Get the atomic informations from a frame's topology
* @param topology The topology
* @param idx The atom index
* @return A pointer to an atom
*/
CHRP_ATOM* chrp_atom(CHRP_FRAME* frame, size_t idx);

/*!
* @brief Get the mass of an atom, in atomic mass units
* @param atom The atom
* @param mass The atom mass
*/
int chrp_atom_mass(const CHRP_ATOM* atom, float* mass);

/*!
* @brief Get the mass of an atom, in atomic mass units
* @param atom The atom
* @param mass The atom mass
*/
int chrp_atom_mass_set(CHRP_ATOM* atom, float mass);

/*!
* @brief Get the charge of an atom, in number of the electron charge e
* @param atom The atom
* @param charge The atom charge
*/
int chrp_atom_charge(const CHRP_ATOM* atom, float* charge);

/*!
* @brief Get the charge of an atom, in number of the electron charge e
* @param atom The atom
* @param charge The atom charge
*/
int chrp_atom_charge_set(CHRP_ATOM* atom, float charge);

/*!
* @brief Get the name of an atom, in atomic mass units
* @param atom The atom
* @param name A string buffer to be filled with the name
* @param buffsize The size of the string buffer
*/
int chrp_atom_name(const CHRP_ATOM* atom, char* name, size_t buffsize);

/*!
* @brief Get the name of an atom, in atomic mass units
* @param atom The atom
* @param name A null terminated string containing the name
* @param buffsize The size of the string buffer
*/
int chrp_atom_name_set(CHRP_ATOM* atom, const char* name);

/*!
* @brief Destroy an atoms, and free the associated memory
* @param atoms The atoms to destroy
* @return The status code
*/
int chrp_atom_free(CHRP_ATOM* atom);


#ifdef __cplusplus
}
#endif

#endif
