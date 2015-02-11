/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_TOPOLOGY_HPP
#define HARP_TOPOLOGY_HPP

#include <array>

#include "Atom.hpp"

namespace harp {

typedef std::array<size_t, 2> bond;
typedef std::array<size_t, 3> angle ;
typedef std::array<size_t, 4> dihedral;

#define bond(x, y) (bond{{(x), (y)}})
#define angle(x, y, z) (angle{{(x), (y), (z)}})
#define dihedral(x, y, z, w) (dihedral{{(x), (y), (z), (w)}})

inline bool operator==(const bond& a, const bond& b){
    return a[0] == b[0] && a[1] == b[1];
}
inline bool operator==(const angle& a, const angle& b){
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}
inline bool operator==(const dihedral& a, const dihedral& b){
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}

using std::vector;
/*!
 * @class Topology Topology.hpp Topology.cpp
 * @brief A topology contains the definition of all the particles in the system.
 *
 * A topology contains the definition of all the particles in the system, and the
 * liaisons between the particles (bonds, angles, dihedrals, ...). Only the atoms
 * and the bonds are stored, the angles and the dihedrals are computed automaticaly.
 */
class Topology {
public:
    //! Construct a topology with capacity for \c natoms atoms.
    explicit Topology(size_t natoms);
    //! Construct an empty topology
    Topology();
    Topology(const Topology &) = default;
    Topology& operator=(const Topology &) = default;
    Atom& operator[](size_t index);

    //! Get a const (non-modifiable) reference to the list of atoms
    const vector<size_t>& atom_list(void) const {return _atoms;}
    //! Get a modifiable reference to the list of atoms
    vector<size_t>& atom_list(void) {return _atoms;}

    //! Get a const (non-modifiable) reference to the list of atom types
    const vector<Atom>& atom_types(void) const {return _atom_types;}
    //! Get a modifiable reference to the list of atoms
    vector<Atom>& atom_types(void) {return _atom_types;}

    //! Add an atom in the system
    void add_atom(Atom at);
    //! Add a bond in the system, between the atoms at index \c atom_i and \c atom_j
    void add_bond(size_t atom_i, size_t atom_j);

    //! Get a reference to the atom at the position \c index
    Atom& atom(size_t index) {return _atom_types[_atoms[index]];}
    //! Get a const (non-modifiable) reference to the atom at the position \c index
    const Atom& atom(size_t index) const {return _atom_types[_atoms[index]];}

    //! Get the number of atoms in the topology
    size_t natoms() {return _atoms.size();}
    //! Reserve space for \c natoms in the topology
    void reserve(size_t natoms) {_atoms.reserve(natoms); _bonds.reserve(natoms);}
    //! Clear the topology
    void clear();

    //! Try to guess the bonds in the system.
    //! TODO
    void guess_bonds() {};

    //! Get the bonds in the system
    vector<bond> bonds(void);
    //! Get the angles in the system
    vector<angle> angles(void);
    //! Get the dihedral angles in the system
    vector<dihedral> dihedrals(void);
private:
    //! Internal list of atoms types
    vector<Atom> _atom_types;
    //! Internal list of atoms. The index refers to the _atom_types list
    vector<size_t> _atoms;
    //! Internal list of liaisons. The vector at index i contains the index of
    //! all the atoms linked to the atom i.
    vector<vector<size_t>> _bonds;
};

//! Create a topology containing \c natoms atoms, all of the UNDEFINED type.
Topology dummy_topology(size_t natoms);

} // namespace harp

#endif
