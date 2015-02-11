/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

/*! @file harp.h
* Harp main C API header.
*
* This file contains all the function definitions for the C API of Harp, and
* should be self-documented enough.
*/

#ifndef HARP_C_API_H
#define HARP_C_API_H

#define uint unsigned int

#ifdef __cplusplus
extern "C" {

    namespace harp {
        class Trajectory;
        class Frame;
        class Atom;
    }
    typedef harp::Trajectory HARP_TRAJECTORY;
    typedef harp::Frame HARP_FRAME;
    typedef harp::Atom HARP_ATOM;
#else
    //! Opaque type handling trajectories files
    typedef struct HARP_TRAJECTORY HARP_TRAJECTORY;
    //! Opaque type handling frames, *i.e* data from a step
    typedef struct HARP_FRAME HARP_FRAME;
    //! Opaque type handling an atom
    typedef struct HARP_ATOM HARP_ATOM;
#endif

    /*!
    * @brief Create an empty frame adaptated to a specific trajectory
    * @param file A trajectory to work with
    * @return A pointer to the frame
    */
    HARP_FRAME* harp_frame(const HARP_TRAJECTORY* file);

    /*!
    * @brief Get the frame size, i.e. the current number of atoms
    * @param frame The frame to analyse
    * @return The number of atoms in the frame
    */
    int harp_frame_size(const HARP_FRAME* frame);

    /*!
    * @brief Get the positions from a frame
    * @param frame The frame
    * @param data A Nx3 float array to be filled with the data
    * @param size The array size (N).
    * @return The status code
    */
    int harp_frame_positions(const HARP_FRAME* frame, float** data, const uint size);

    /*!
    * @brief Get the velocities from a frame, if they exists
    * @param frame The frame
    * @param data A Nx3 float array to be filled with the data
    * @param size The array size (N).
    * @return The status code
    */
    int harp_frame_velocities(const HARP_FRAME* frame, float** data, const uint size);

    /*!
    * @brief Get the atomic informations from a file
    * @param frame The frame
    * @param idx The atom index
    * @return A pointer to an atom
    */
    HARP_ATOM* harp_get_atom(const HARP_FRAME* frame, const int idx);

    /*!
    * @brief Destroy a frame
    * @param frame The frame to destroy
    * @return The status code
    */
    int harp_frame_delete(HARP_FRAME* frame);

    /**************************************************************************/

    /*!
    * @brief Get the mass of an atom, in atomic mass units
    * @param atom The atom
    * @return The atom mass
    */
    float harp_get_mass(const HARP_ATOM* atom);

    /*!
    * @brief Get the charge of an atom, in number of the electron charge e
    * @param atom The atom
    * @return The atom charge
    */
    float harp_get_charge(const HARP_ATOM* atom);

    /*!
    * @brief Get the Van der Waals radius of an atom, in nanometers
    * @param atom The atom
    * @return The atom radius
    */
    float harp_get_radius(const HARP_ATOM* atom);

    /*!
    * @brief Get the name of an atom, in atomic mass units
    * @param atom The atom
    * @param name A string buffer to be filled with the name
    * @param buffsize The size of the string buffer
    * @return The string buffer
    */
    char* harp_get_name(const HARP_ATOM* atom, char* name, const uint buffsize);

    /**************************************************************************/

    /*!
    * @brief Open a trajectory file.
    * @param filename The filename
    * @param mode The opening ("r" or "w") for the file. The default is read mode.
    * @return A pointer to the file
    */
    HARP_TRAJECTORY* harp_open(const char* filename, const char* mode);

    /*!
    * @brief Read a specific step of a trajectory in a frame
    * @param file A pointer to the file
    * @param step The step to read
    * @param frame A frame to fill with the data
    * @return TODO
    */
    int harp_read_step(const HARP_TRAJECTORY *file, const int step, HARP_FRAME* frame);

    /*!
    * @brief Read the next step of a trajectory in a frame
    * @param file A pointer to the file
    * @param frame A frame to fill with the data
    * @return TODO
    */
    int harp_read_next_step(const HARP_TRAJECTORY *file, HARP_FRAME *frame);

    /*!
    * @brief Close a trajectory file
    * @param file A pointer to the file
    * @return The status code
    */
    int harp_close(HARP_TRAJECTORY *file);

#ifdef __cplusplus
}
#endif

#undef uint

#endif
