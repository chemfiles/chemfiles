// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_FRAME_HPP
#define CHEMFILES_FRAME_HPP

#include "chemfiles/types.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"

#include "chemfiles/exports.hpp"
#include "chemfiles/optional.hpp"

namespace chemfiles {

/// A frame contains data from one simulation step
/// The Frame class holds data from one step of a simulation: the current
/// topology, the positions, and the velocities of the particles in the system.
/// If some information is missing (topology or velocity or unit cell), the
/// corresponding data is filled with a default value. Specifically:
///
/// - `velocities` is the `nullopt` version of `optional<Array3D>`. Here,
///    `optional<T>` refers to the optional template as defined in
///    [std::experimental::optional][optional]
/// - `cell` is an infinite unit cell;
/// - `topology` is empty, and contains no data.
///
/// [optional]: http://en.cppreference.com/w/cpp/experimental/optional
class CHFL_EXPORT Frame {
public:
    /// Default constructor
    Frame();
    /// Constructor reserving some space for `natoms`
    explicit Frame(size_t natoms);
    /// Constructor reserving space for `topology.natoms()`, and using `cell`
    /// as unit cell. `cell` default to an `INFINITE` unit cell.
    explicit Frame(Topology topology, UnitCell cell = UnitCell());

    Frame(Frame&&) = default;
    Frame& operator=(Frame&&) = default;

    /// Get a clone (exact copy) of this frame.
    ///
    /// This replace the implicit copy constructor (which is disabled) to
    /// make an explicit copy of the frame.
    Frame clone() const {
        return *this;
    }

    /// Get a modifiable reference to the positions
    Span3D positions() { return positions_; }
    /// Get a const (non modifiable) reference to the positions
    const Array3D& positions() const { return positions_; }

    /// Get an optional modifiable reference to the velocities
    optional<Span3D> velocities() {
        return velocities_ ? optional<Span3D>(as_span(*velocities_))
                           : optional<Span3D>(nullopt);
    }
    /// Get an optional const (non modifiable) reference to the velocities
    const optional<Array3D>& velocities() const { return velocities_; }
    /// Add velocities to this frame. If velocities are already defined,
    /// this functions does nothing.
    void add_velocities();

    /// Get a modifiable reference to the internal topology
    Topology& topology() { return topology_; }
    /// Get a const (non-modifiable) reference to the internal topology
    const Topology& topology() const { return topology_; }
    /// Set the system topology
    void set_topology(const Topology& topology);

    /// Get a const (non-modifiable) reference to the unit cell of the system
    const UnitCell& cell() const { return cell_; }
    UnitCell& cell() { return cell_; }
    /// Set the unit cell fo the system
    void set_cell(const UnitCell& c) { cell_ = c; }

    /// Resize the frame to store data for `natoms` atoms. If the new size is
    /// bigger than the old one, missing data is initializd to 0. Pre-existing
    /// values are conserved.
    /// This function only resize the velocities if the data is present.
    void resize(size_t natoms);

    /// Reserve size in the frame to store data for `natoms` atoms.
    /// This function only reserve storage for the the velocities if the data
    /// is present.
    void reserve(size_t natoms);

    /// Add an `atom` at the given `position` and optionally with the given
    /// `velocity`. The `velocity` value will only be used if this frame
    /// contains velocity data.
    void add_atom(Atom atom, Vector3D position, Vector3D velocity = Vector3D());

    /// Get the number of atoms in the system
    size_t natoms() const;

    /// Remove the atom at index `i` in the system. `i` must be lower than
    /// `natoms()`.
    void remove(size_t i);

    /// Get the current simulation step
    size_t step() const { return step_; }
    /// Set the current simulation step
    void set_step(size_t s) { step_ = s; }

    /// Guess the bonds, angles and dihedrals in the system. The bonds are
    /// guessed using a distance-based algorithm, and then angles and dihedrals
    /// are guessed from the bonds.
    void guess_topology();

private:
    Frame(const Frame&) = default;
    Frame& operator=(const Frame&) = default;

    /// Current simulation step
    size_t step_;
    /// Positions of the particles
    Array3D positions_;
    /// Velocities of the particles
    optional<Array3D> velocities_;
    /// Topology of the described system
    Topology topology_;
    /// Unit cell of the system
    UnitCell cell_;
};

} // namespace chemfiles

#endif
