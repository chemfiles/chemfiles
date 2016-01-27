/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_FRAME_HPP
#define CHEMFILES_FRAME_HPP

#include "chemfiles/Array3D.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"

#include "chemfiles/exports.hpp"
#include "chemfiles/optional.hpp"

namespace chemfiles {

/*!
 * @class Frame Frame.hpp Frame.cpp
 * @brief A frame contains data from one simulation step
 *
 * The Frame class holds data from one step of a simulation: the current topology,
 * the positions, and the velocities of the particles in the system. If some information
 * is missing (topology or velocity or unit cell), the corresponding data is filled with
 * a default value. Specifically:
 * 	- `velocities` is the `nullopt` version of `optional<Array3D>`. Here, `optional<T>`
 * 	  refers to the optional template as defined in [std::experimental::optional](http://en.cppreference.com/w/cpp/experimental/optional)
 * 	- `cell` is an infinite unit cell;
 * 	- `topology` is empty, and contains no data.
 */
class CHFL_EXPORT Frame {
public:
    //! Default constructor
    Frame();
    //! Constructor reserving some space for \c natoms
    explicit Frame(size_t natoms);
    //! Constructor reserving space for `topology.natoms()`, and using `cell`
    //! as unit cell. `cell` default to an `INFINITE` unit cell.
    explicit Frame(const Topology& topology, const UnitCell& cell = UnitCell());

    //! Get a modifiable reference to the positions
    Array3D& positions() {return positions_;}
    //! Get a const (non modifiable) reference to the positions
    const Array3D& positions() const {return positions_;}
    //! Set the positions
    void set_positions(const Array3D& pos) {positions_ = pos;}

    //! Get an optional modifiable reference to the velocities
    optional<Array3D>& velocities() {return velocities_;}
    //! Get an optional const (non modifiable) reference to the velocities
    const optional<Array3D>& velocities() const {return velocities_;}
    //! Set the velocities to `vel`
    void set_velocities(const Array3D& vel) {velocities_.emplace(vel);}

    //! Get the number of particles in the system
    size_t natoms() const;

    //! Get a modifiable reference to the internal topology
    Topology& topology() {return topology_;}
    //! Get a const (non-modifiable) reference to the internal topology
    const Topology& topology() const {return topology_;}
    //! Set the system topology
    void set_topology(const Topology& top) {topology_ = top;}

    //! Get a const (non-modifiable) reference to the unit cell of the system
    const UnitCell& cell() const {return cell_;}
    UnitCell& cell() {return cell_;}
    //! Set the unit cell fo the system
    void set_cell(const UnitCell& c) {cell_ = c;}

    //! Resize the frame to store data for `natoms` atoms. If the new size is bigger than
    //! the old one, missing data is initializd to 0. Pre-existing values are conserved.
    //! This function only resize the velocities if the data is present.
    void resize(size_t natoms);

    //! Get the current simulation step
    size_t step() const {return step_;}
    //! Set the current simulation step
    void set_step(size_t s) {step_ = s;}

    //! Try to guess the bonds, angles and dihedrals in the system. If \c bonds
    //! is true, guess everything; else only guess the angles and dihedrals from
    //! the bond list.
    void guess_topology(bool bonds = true);
private:
    //! Guess the bond list and add it to the internal topology
    void guess_bonds();

    //! Current simulation step
    size_t step_;
    //! Positions of the particles
    Array3D positions_;
    //! Velocities of the particles
    optional<Array3D> velocities_;
    //! Topology of the described system
    Topology topology_;
    //! Unit cell of the system
    UnitCell cell_;
};

} // namespace chemfiles

#endif
