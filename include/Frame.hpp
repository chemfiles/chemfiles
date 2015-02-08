/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_FRAME_HPP
#define HARP_FRAME_HPP

#include <array>
#include <vector>

#include "Topology.hpp"
#include "UnitCell.hpp"

namespace harp {

//! Fixed-size array of 3 components: x, y and z values.
typedef std::array<float, 3> Vector3D;
//! Variable-size array of vector of 3 components
typedef std::vector<Vector3D> Array3D;

#define Vector3D(x, y, z) (Vector3D{{(x), (y), (z)}})

/*!
 * @class Frame Frame.hpp Frame.cpp
 * @brief A frame contains data from one simulation step
 *
 * The Frame class holds data from one step of a simulation: the current topology,
 * the positions, and maybe the velocities of the particles in the system.
 */
class Frame {
public:
    //! Default constructor, reserving space for 100 atoms
    Frame();
    //! Constructor reserving some space for \c natoms
    explicit Frame(size_t natoms);
    //! Constructor called to build a frame that can hold the data from a
    //! specific topology.
    explicit Frame(Topology top, bool has_velocities = false);
    ~Frame(){}

    //! Get a modifiable reference to the positions
    Array3D& positions() {return _positions;}
    //! Get a const (non modifiable) reference to the positions
    const Array3D& positions() const {return _positions;}

    //! Does this frame have velocity data ?
    bool has_velocities();

    //! Get a modifiable reference to the velocities
    Array3D& velocities() {return _velocities;}
    //! Get a const (non modifiable) reference to the velocities
    const Array3D& velocities() const {return _velocities;}

    //! Get a *copy* of the positions, as a C-style array. The array is assumed
    //! to have a shape (size x 3); i.e. pos[size][3]. The \c size should be
    //! equal to the number of particles in the system.
    void raw_positions(float pos[][3], size_t size) const;
    //! Get a *copy* of the velocities, as a C-style array. The array is assumed
    //! to have a shape (size x 3); i.e. vel[size][3]. The \c size should be
    //! equal to the number of particles in the system.
    void raw_velocities(float vel[][3], size_t size) const;

    //! Get the number of particles in the system
    size_t natoms() const;

    //! Get a modifiable reference to the internal topology
    Topology& topology() {return _topology;}
    //! Get a const (non-modifiable) reference to the internal topology
    const Topology& topology() const {return _topology;}

    //! Get a const (non-modifiable) reference to the unit cell of the system
    const UnitCell& cell() const {return _cell;}
    //! Set the unit cell fo the system
    void cell(const UnitCell& c) {_cell = c;}

    //! Reserve some size in the internal arrays, and initialize the array with 0.
    //! The \c reserve_velocities parameter should be \c true to reserve space for
    //! the velocities.
    void reserve(size_t size, bool reserve_velocities = false);

    //! Get the current simulation step
    size_t step() const {return _step;}
    //! Set the current simulation step
    void step(size_t s) {_step = s;}

private:
    //! Current simulation step
    size_t _step;
    //! Positions of the particles
    Array3D _positions;
    //! Velocities of the particles
    Array3D _velocities;
    //! Topology of the described system
    Topology _topology;
    //! Unit cell of the system
    UnitCell _cell;
};

} // namespace harp

#endif
