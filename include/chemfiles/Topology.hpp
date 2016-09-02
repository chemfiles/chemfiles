/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_TOPOLOGY_HPP
#define CHEMFILES_TOPOLOGY_HPP

#include <vector>

#include "chemfiles/Atom.hpp"
#include "chemfiles/Connectivity.hpp"
#include "chemfiles/exports.hpp"

namespace chemfiles {

/*! @class Topology Topology.hpp Topology.cpp
 * A topology contains the definition of all the atoms in the system, and
 * the liaisons between the particles (bonds, angles, dihedrals, ...).
 *
 * Only the atoms and the bonds are stored, the angles and the dihedrals are
 * computed automaticaly.
 *
 * Iterating over a Topology will yield the atoms in the system.
 */
class CHFL_EXPORT Topology {
public:
    using iterator = std::vector<Atom>::iterator;
    using const_iterator = std::vector<Atom>::const_iterator;

    //! Construct an empty topology
    Topology() {}
    Topology(const Topology&) = default;
    Topology& operator=(const Topology&) = default;
    Topology(Topology&&) = default;
    Topology& operator=(Topology&&) = default;

    //! Get a reference to the atom at the position `index`
    Atom& operator[](size_t index) { return atoms_[index]; }
    //! Get a const (non-modifiable) reference to the atom at the position
    //! `index`
    const Atom& operator[](size_t index) const { return atoms_[index]; }

    iterator begin() {return atoms_.begin();}
    const_iterator begin() const {return atoms_.begin();}
    const_iterator cbegin() const {return atoms_.cbegin();}
    iterator end() {return atoms_.end();}
    const_iterator end() const {return atoms_.end();}
    const_iterator cend() const {return atoms_.cend();}

    //! Add an atom in the system
    void append(Atom atom);
    //! Delete an atom in the system. If `idx` is out of bounds, do nothing.
    void remove(size_t idx);
    //! Add a bond in the system, between the atoms at index `atom_i` and
    //! `atom_j`
    void add_bond(size_t atom_i, size_t atom_j) {
        connect_.add_bond(atom_i, atom_j);
    }
    //! Remove a bond in the system, between the atoms at index `atom_i` and
    //! `atom_j`
    void remove_bond(size_t atom_i, size_t atom_j) {
        connect_.remove_bond(atom_i, atom_j);
    }

    //! Get the number of atoms in the topology
    size_t natoms() const { return atoms_.size(); }
    //! Resize the topology to hold `natoms` atoms, adding `UNDEFINED` atoms
    //! as needed.
    void resize(size_t natoms);
    //! Reserve size in the topology to store data for `natoms` atoms.
    void reserve(size_t natoms);

    //! Check wether the atoms at indexes `i` and `j` are bonded or not
    bool isbond(size_t i, size_t j) const;
    //! Check wether the atoms at indexes `i`, `j` and `k` constitues an
    //! angle
    bool isangle(size_t i, size_t j, size_t k) const;
    //! Check wether the atoms at indexes `i` `j`, `k` and `m` constitues a
    //! dihedral angle
    bool isdihedral(size_t i, size_t j, size_t k, size_t m) const;

    //! Get the bonds in the system
    std::vector<Bond> bonds() const;
    //! Get the angles in the system
    std::vector<Angle> angles() const;
    //! Get the dihedral angles in the system
    std::vector<Dihedral> dihedrals() const;

    //! Remove all bonding information in the topology (bonds, angles and
    //! dihedrals)
    void clear_bonds() { connect_ = Connectivity(); }

private:
    //! Atoms in the system.
    std::vector<Atom> atoms_;
    //! Connectivity of the system. All the indices refers to the positions in
    //! `atoms_`
    Connectivity connect_;
};

} // namespace chemfiles

#endif
