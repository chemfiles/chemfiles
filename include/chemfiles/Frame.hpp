// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FRAME_HPP
#define CHEMFILES_FRAME_HPP

#include "chemfiles/exports.hpp"
#include "chemfiles/types.hpp"
#include "chemfiles/external/span.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"

namespace chemfiles {

/// A frame contains data from one simulation step The Frame class holds data
/// from one step of a simulation: the current topology, the positions, and the
/// velocities of the particles in the system. If some information is missing
/// the corresponding data is filled with a default value. Specifically:
///
/// @verbatim embed:rst:leading-slashes
///
/// * ``cell`` is an infinite unit cell;
/// * ``topology`` is empty, and contains no data;
/// * ``positions`` is filled with zeros;
/// * ``velocities`` is the nullopt variant of :cpp:class:`chemfiles::optional`.
///
/// @endverbatim
///
///Iterating over a `Frame` will yield all the atoms in the system.
///
/// @example{tests/doc/frame/iterate.cpp}
class CHFL_EXPORT Frame final {
public:
    /// Create an empty frame with no atoms and an infinite unit cell.
    ///
    /// @example{tests/doc/frame/frame-0.cpp}
    Frame();

    /// Create a frame containing the atoms in the `topology`, and the given
    /// unit `cell`.
    ///
    /// The positions of each atom will be initiliazed to `(0, 0, 0)`, and the
    /// frame will not contain any velocity.
    ///
    /// @example{tests/doc/frame/frame-2.cpp}
    ///
    /// @param topology `Topology` containing the atoms to use in this frame
    /// @param cell `UnitCell` bounding the new frame. It default to an
    /// `INFINITE` unit cell.
    explicit Frame(Topology topology, UnitCell cell = UnitCell());

    ~Frame() = default;
    Frame(Frame&&) = default;
    Frame& operator=(Frame&&) = default;

    /// Get a clone (exact copy) of this frame.
    ///
    /// This replace the implicit copy constructor (which is private) to
    /// make an explicit copy of the frame.
    ///
    /// @example{tests/doc/frame/clone.cpp}
    Frame clone() const {
        return *this;
    }

    /// Get a const reference to the topology of this frame
    ///
    /// It is not possible to get a modifiable reference to the topology,
    /// because it would then be possible to remove/add atoms without changing
    /// the actual positions and velocity storage. Instead, all the mutating
    /// functionalities of the topology are mirrored on the frame (adding and
    /// removing bonds, adding residues, *etc.*)
    ///
    /// @example{tests/doc/frame/topology.cpp}
    const Topology& topology() const {
        return topology_;
    }

    /// Set the topology of this frame to `topology`
    ///
    /// @param topology the new Topology to use for this frame
    ///
    /// @throw Error if the topology size does not match the size of this frame
    ///
    /// @example{tests/doc/frame/topology.cpp}
    void set_topology(Topology topology);

    /// Get a const reference to the unit cell of this frame
    ///
    /// @example{tests/doc/frame/cell.cpp}
    const UnitCell& cell() const {
        return cell_;
    }

    /// Get a reference to the unit cell of this frame
    ///
    /// @example{tests/doc/frame/cell.cpp}
    UnitCell& cell() {
        return cell_;
    }

    /// Set the unit cell for this frame to `cell`
    ///
    /// @param cell the new UnitCell to use for this frame
    ///
    /// @example{tests/doc/frame/cell.cpp}
    void set_cell(UnitCell cell) {
        cell_ = cell;
    }

    /// Get the number of atoms in this frame
    ///
    /// @example{tests/doc/frame/size.cpp}
    size_t size() const;

    /// Get the positions of the atoms in this frame.
    ///
    /// @verbatim embed:rst:leading-slashes
    /// A :cpp:class:`chemfiles::span` is a view inside a vector allowing
    /// mutation of the values, but no memory allocation.
    /// @endverbatim
    ///
    /// @example{tests/doc/frame/positions.cpp}
    span<Vector3D> positions() {
        return positions_;
    }

    /// Get the positions in this frame as a const reference
    ///
    /// @example{tests/doc/frame/positions.cpp}
    const std::vector<Vector3D>& positions() const {
        return positions_;
    }

    /// Add velocities data storage to this frame.
    ///
    /// If velocities are already defined, this functions does nothing. The new
    /// velocities are initialized to 0.
    ///
    /// @example{tests/doc/frame/add_velocities.cpp}
    void add_velocities();

    /// Get an velocities of the atoms in this frame, if this frame contains
    /// velocity data.
    ///
    /// @verbatim embed:rst:leading-slashes
    /// This function returna an :cpp:class:`chemfiles::optional` value that is
    /// close to C++17 ``std::optional``.
    ///
    /// A :cpp:class:`chemfiles::span` is a view inside a vector allowing
    /// mutation of the values, but no memory allocation.
    /// @endverbatim
    ///
    /// @example{tests/doc/frame/velocities.cpp}
    optional<span<Vector3D>> velocities() {
        if (velocities_) {
            return {*velocities_};
        } else {
            return nullopt;
        }
    }

    /// Get an velocities of the atoms in this frame as a const reference, if
    /// this frame contains velocity data.
    ///
    /// @example{tests/doc/frame/velocities.cpp}
    optional<const std::vector<Vector3D>&> velocities() const {
        if (velocities_) {
            return {*velocities_};
        } else {
            return nullopt;
        }
    }

    /// Resize the frame to contain `size` atoms.
    ///
    /// If the new number of atoms is bigger than the old one, missing data is
    /// initializd to 0. Pre-existing values are conserved.
    ///
    /// If the new size if smaller than the old one, all atoms and connectivity
    /// elements after the new size are removed.
    ///
    /// @example{tests/doc/frame/resize.cpp}
    void resize(size_t size);

    /// Allocate memory in the frame to have enough size for `size` atoms.
    ///
    /// This function does not change the actual number of atoms in the frame,
    /// and should be used as an optimisation.
    ///
    /// @example{tests/doc/frame/reserve.cpp}
    void reserve(size_t size);

    /// Add an `atom` at the given `position` and optionally with the given
    /// `velocity`. The `velocity` value will only be used if this frame
    /// contains velocity data.
    ///
    /// @example{tests/doc/frame/add_atom.cpp}
    void add_atom(Atom atom, Vector3D position, Vector3D velocity = Vector3D());

    /// Remove the atom at index `i` in the system.
    ///
    /// @throws chemfiles::OutOfBounds if `i` is bigger than the number of atoms
    ///         in this frame
    ///
    /// @example{tests/doc/frame/remove.cpp}
    void remove(size_t i);

    /// Get the current simulation step.
    ///
    /// The step is set by the `Trajectory` when reading a frame.
    ///
    /// @example{tests/doc/frame/step.cpp}
    size_t step() const {
        return step_;
    }

    /// Set the current simulation step to `step`
    ///
    /// @example{tests/doc/frame/step.cpp}
    void set_step(size_t step) {
        step_ = step;
    }

    /// Guess the bonds, angles, dihedrals and impropers angles in this frame.
    ///
    /// The bonds are guessed using a distance-based algorithm, and then angles,
    /// dihedrals and impropers are guessed from the bonds.
    ///
    /// @throw Error if the Van der Waals radius in unknown for a given atom.
    ///
    /// @example{tests/doc/frame/guess_topology.cpp}
    void guess_topology();

    /// Remove all connectivity information in the frame's topology
    ///
    /// @example{tests/doc/frame/clear_bonds.cpp}
    void clear_bonds() {
        topology_.clear_bonds();
    }

    /// Add a `residue` to this frame's topology.
    ///
    /// @example{tests/doc/frame/add_residue.cpp}
    ///
    /// @param residue the residue to add to this topology
    /// @throw chemfiles::Error if any atom in the `residue` is already in
    ///        another residue in this topology. In that case, the topology is
    ///        not modified.
    void add_residue(Residue residue) {
        topology_.add_residue(std::move(residue));
    }

    /// Add a bond in the system, between the atoms at index `atom_i` and
    /// `atom_j`.
    ///
    /// @example{tests/doc/frame/add_bond.cpp}
    ///
    /// @param atom_i the index of the first atom in the bond
    /// @param atom_j the index of the second atom in the bond
    /// @throws OutOfBounds if `atom_i` or `atom_j` are greater than `size()`
    /// @throws Error if `atom_i == atom_j`, as this is an invalid bond
    void add_bond(size_t atom_i, size_t atom_j) {
        topology_.add_bond(atom_i, atom_j);
    }

    /// Remove a bond in the system, between the atoms at index `atom_i` and
    /// `atom_j`.
    ///
    /// If the bond does not exist, this does nothing.
    ///
    /// @example{tests/doc/frame/remove_bond.cpp}
    ///
    /// @param atom_i the index of the first atom in the bond
    /// @param atom_j the index of the second atom in the bond
    /// @throws OutOfBounds if `atom_i` or `atom_j` are greater than `size()`
    void remove_bond(size_t atom_i, size_t atom_j) {
        topology_.remove_bond(atom_i, atom_j);
    }

    /// Get a reference to the atom at the position `index`.
    ///
    /// @example{tests/doc/frame/index.cpp}
    ///
    /// @param index the atomic index
    /// @throws OutOfBounds if `index` is greater than `size()`
    Atom& operator[](size_t index) {
        return topology_[index];
    }

    /// Get a const reference to the atom at the position `index`.
    ///
    /// @example{tests/doc/frame/index.cpp}
    ///
    /// @param index the atomic index
    /// @throws OutOfBounds if `index` is greater than `size()`
    const Atom& operator[](size_t index) const {
        return topology_[index];
    }

    using iterator = Topology::iterator;
    using const_iterator = Topology::const_iterator;
    iterator begin() {return topology_.begin();}
    const_iterator begin() const {return topology_.begin();}
    const_iterator cbegin() const {return topology_.cbegin();}
    iterator end() {return topology_.end();}
    const_iterator end() const {return topology_.end();}
    const_iterator cend() const {return topology_.cend();}

    /// Get the distance between the atoms at indexes `i` and `j`, accounting
    /// for periodic boundary conditions. The distance is expressed in angstroms.
    ///
    /// @throws chemfiles::OutOfBounds if `i` or `j` are bigger than the number
    ///         of atoms in this frame
    ///
    /// @example{tests/doc/frame/distance.cpp}
    double distance(size_t i, size_t j) const;

    /// Get the angle formed by the atoms at indexes `i`, `j` and `k`,
    /// accounting for periodic boundary conditions. The angle is expressed in
    /// radians.
    ///
    /// @throws chemfiles::OutOfBounds if `i`, `j` or `k` are bigger than the
    ///         number of atoms in this frame
    ///
    /// @example{tests/doc/frame/angle.cpp}
    double angle(size_t i, size_t j, size_t k) const;

    /// Get the dihedral angle formed by the atoms at indexes `i`, `j`, `k` and
    /// `m`, accounting for periodic boundary conditions. The angle is expressed
    /// in radians.
    ///
    /// @throws chemfiles::OutOfBounds if `i`, `j`, `k` or `m` are bigger than
    ///         the number of atoms in this frame
    ///
    /// @example{tests/doc/frame/dihedral.cpp}
    double dihedral(size_t i, size_t j, size_t k, size_t m) const;

    /// Get the out of plane distance formed by the atoms at indexes `i`, `j`,
    /// `k` and `m`, accounting for periodic boundary conditions. The distance
    /// is expressed in angstroms.
    ///
    /// This is the distance betweent the atom j and the ikm plane. The j atom
    /// is the center of the improper dihedral angle formed by i, j, k and m.
    ///
    /// @throws chemfiles::OutOfBounds if `i`, `j`, `k` or `m` are bigger than
    ///         the number of atoms in this frame
    ///
    /// @example{tests/doc/frame/out_of_plane.cpp}
    double out_of_plane(size_t i, size_t j, size_t k, size_t m) const;

    /// Set an arbitrary property for this frame with the given `name` and
    /// `value`. If a property with this name already exist, it is silently
    /// replaced with the new value.
    ///
    /// @example{tests/doc/frame/set.cpp}
    void set(std::string name, Property value);

    /// Get the property with the given `name` for this frame if it exists.
    ///
    /// If no property with the given `name` is found, this function returns
    /// `nullopt`.
    ///
    /// @verbatim embed:rst:leading-slashes
    /// This function returna an :cpp:class:`chemfiles::optional` value that is
    /// close to C++17 ``std::optional``.
    /// @endverbatim
    ///
    /// @example{tests/doc/frame/get.cpp}
    optional<const Property&> get(const std::string& name) const;

private:
    Frame(const Frame&) = default;
    Frame& operator=(const Frame&) = default;

    /// Current simulation step
    size_t step_ = 0;
    /// Positions of the particles
    std::vector<Vector3D> positions_;
    /// Velocities of the particles
    optional<std::vector<Vector3D>> velocities_;
    /// Topology of the described system
    Topology topology_;
    /// Unit cell of the system
    UnitCell cell_;
    /// Properties stored in this frame
    property_map properties_;
};

} // namespace chemfiles

#endif
