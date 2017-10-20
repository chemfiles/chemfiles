// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_TOPOLOGY_HPP
#define CHEMFILES_TOPOLOGY_HPP

#include <vector>
#include <unordered_map>

#include "chemfiles/Atom.hpp"
#include "chemfiles/Connectivity.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/exports.hpp"

#include "chemfiles/optional.hpp"

namespace chemfiles {

/// A topology contains the definition of all the atoms in the system, and
/// the liaisons between the particles (bonds, angles, dihedrals, ...).
///
/// Only the atoms and the bonds are stored, the angles and the dihedrals are
/// computed automaticaly.
///
/// Iterating over a `Topology` will yield the atoms in the system.
class CHFL_EXPORT Topology {
public:
    using iterator = std::vector<Atom>::iterator;
    using const_iterator = std::vector<Atom>::const_iterator;

    /// Construct an empty topology
    Topology() {}
    Topology(const Topology&) = default;
    Topology& operator=(const Topology&) = default;
    Topology(Topology&&) = default;
    Topology& operator=(Topology&&) = default;

    /// Get a reference to the atom at the position `index`.
    ///
    /// @throws OutOfBounds if `index` is greater than `natoms()`
    Atom& operator[](size_t index) {
        if (index >= natoms()) {
            throw OutOfBounds(
                "Atomic index out of bounds in topology: we have "
                + std::to_string(natoms()) + " atoms, but the index is "
                + std::to_string(index)
            );
        }
        return atoms_[index];
    }

    /// Get a const (non-modifiable) reference to the atom at the position
    /// `index`.
    ///
    /// @throws OutOfBounds if `index` is greater than `natoms()`
    const Atom& operator[](size_t index) const {
        if (index >= natoms()) {
            throw OutOfBounds(
                "Atomic index out of bounds in topology: we have "
                + std::to_string(natoms()) + " atoms, but the index is "
                + std::to_string(index)
            );
        }
        return atoms_[index];
    }

    iterator begin() {return atoms_.begin();}
    const_iterator begin() const {return atoms_.begin();}
    const_iterator cbegin() const {return atoms_.cbegin();}
    iterator end() {return atoms_.end();}
    const_iterator end() const {return atoms_.end();}
    const_iterator cend() const {return atoms_.cend();}

    /// Add an `atom` at the end of this topology.
    void add_atom(Atom atom);

    /// Delete the atom at index `i` in the system.
    ///
    /// @throws OutOfBounds if `i` is greater than natoms()
    void remove(size_t i);

    /// Add a bond in the system, between the atoms at index `atom_i` and
    /// `atom_j`.
    ///
    /// @throws OutOfBounds if `atom_i` or `atom_j` are greater than `natoms()`
    void add_bond(size_t atom_i, size_t atom_j);

    /// Remove a bond in the system, between the atoms at index `atom_i` and
    /// `atom_j`.
    ///
    /// @throws OutOfBounds if `atom_i` or `atom_j` are greater than `natoms()`
    void remove_bond(size_t atom_i, size_t atom_j);

    /// Get the number of atoms in the topology
    size_t natoms() const { return atoms_.size(); }

    /// Resize the topology to hold `natoms` atoms, adding `UNDEFINED` atoms
    /// as needed.
    void resize(size_t natoms);

    /// Reserve size in the topology to store data for `natoms` atoms.
    void reserve(size_t natoms);

    /// Get the bonds in the system, in a sorted vector
    const std::vector<Bond>& bonds() const;
    /// Get the angles in the system, in a sorted vector
    const std::vector<Angle>& angles() const;
    /// Get the dihedral angles in the system, in a sorted vector
    const std::vector<Dihedral>& dihedrals() const;
    /// Get the improper dihedral angles in the system, in a sorted vector
    const std::vector<Improper>& impropers() const;

    /// Remove all bonding information in the topology (bonds, angles and
    /// dihedrals)
    void clear_bonds() { connect_ = Connectivity(); }

    /// Add a `residue` to this topology.
    ///
    /// This function throws a `chemfiles::Error` if any atom in the `residue`
    /// is already in another residue in this topology. In that case, the
    /// topology is not modified.
    void add_residue(Residue residue);

    /// Check if two residues are linked together, i.e. if there is a bond
    /// between one atom in the `first` residue and one atom in the `second`
    /// one. Both residues should be in this topology.
    ///
    /// If `first == second`, this function returns `true`.
    bool are_linked(const Residue& first, const Residue& second) const;

    /// Get the residue containing the `atom` at the given index.
    optional<const Residue&> residue(size_t atom) const;

    /// Get all the residues in the topology
    const std::vector<Residue>& residues() const {
        return residues_;
    }

private:
    /// Check wether the atoms at indexes `i` and `j` are bonded or not
    bool is_bond(size_t i, size_t j) const;

    /// Atoms in the system.
    std::vector<Atom> atoms_;
    /// Connectivity of the system. All the indexes refers to the positions in
    /// `atoms_`
    Connectivity connect_;
    /// List of residues in the system. All the indexes refers to the positions
    /// in `atoms_`
    std::vector<Residue> residues_;
    /// Association between atom indexes and residues indexes.
    std::unordered_map<size_t, size_t> residue_mapping_;
};

} // namespace chemfiles

#endif
