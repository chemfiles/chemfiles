/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_TOPOLOGY_HPP
#define HARP_TOPOLOGY_HPP

#include <cassert>
#include <array>
#include <vector>
#include <unordered_set>
#include "Atom.hpp"

namespace harp {

//! The bond struct ensure a canonical representation of a bond between atoms
//! i and j, with i<j
struct bond {
    bond(size_t first, size_t second){
        assert(first != second);
        _data[0] = std::min(first, second);
        _data[1] = std::max(first, second);
    }
    //! Indexing operator
    size_t& operator[](size_t i) {return _data[i];}
    const size_t& operator[](size_t i) const {return _data[i];}
    //! Comparison operator
    bool operator==(const bond& other) const{
        return _data[0] == other[0] && _data[1] == other[1];
    }
private:
    std::array<size_t, 2> _data;
};

//! The angle struct ensure a canonical representation of an angle between the
//! atoms i, j and k, with i < k
struct angle {
    angle(size_t first, size_t midle, size_t last){
        assert(first != last);
        assert(first != midle);
        _data[0] = std::min(first, last);
        _data[1] = midle;
        _data[2] = std::max(first, last);
    }
    //! Indexing operator
    size_t& operator[](size_t i) {return _data[i];}
    const size_t& operator[](size_t i) const {return _data[i];}
    //! Comparison operator
    bool operator==(const angle& other) const {
        return _data[0] == other[0] && _data[1] == other[1] && _data[2] == other[2];
    }
private:
    std::array<size_t, 3> _data;
};

//! The dihedral struct ensure a canonical representation of a dihedral angle
//! between the atoms i, j, k and m, with max(i, j) < max(k, m))
struct dihedral {
    dihedral(size_t first, size_t second, size_t third, size_t fourth){
        assert(first != second);
        assert(second != third);
        assert(third != fourth);

        if (std::max(first, second) < std::max(third, fourth)) {
            _data[0] = first;
            _data[1] = second;
            _data[2] = third;
            _data[3] = fourth;
        } else {
            _data[0] = fourth;
            _data[1] = third;
            _data[2] = second;
            _data[3] = first;
        }
    }
    //! Indexing operator
    size_t& operator[](size_t i) {return _data[i];}
    const size_t& operator[](size_t i) const {return _data[i];}
    //! Comparison operator
    bool operator==(const dihedral& other) const {
        return _data[0] == other[0] && _data[1] == other[1] &&
               _data[2] == other[2] && _data[3] == other[3];
    }
private:
    std::array<size_t, 4> _data;
};


// We need a hashing function for the std::unordered_set, but it will not be used.
// We will use the operator== instead to ensure the element unicity in the sets.
struct hash {
    std::size_t operator()(bond const& ) const {
        return 42;
    }
    std::size_t operator()(angle const& ) const {
        return 42;
    }
    std::size_t operator()(dihedral const& ) const {
        return 42;
    }
};

/*!
 * @class connectivity Topology.hpp Topology.cpp
 *
 * The connectivity struct store a cache of the bonds, angles and dihedrals
 * in the system. The \c recalculate function should be called when bonds are
 * added or removed. The \c bonds set is the main source of information, all the
 * other data are cached from it.
 */
class Connectivity {
public:
    //! Recalculate the angles and the dihedrals from the bond list
    void recalculate() const;
    //! Clear all the content
    void clear();
    //! Access the underlying data
    const std::unordered_set<bond, hash>& bonds() const {return _bonds;}
    const std::unordered_set<angle, hash>& angles() const;
    const std::unordered_set<dihedral, hash>& dihedrals() const;
    //! Add a bond between the atoms \c i and \c j
    void add_bond(size_t i, size_t j);
    //! Remove any bond between the atoms \c i and \c j
    void remove_bond(size_t i, size_t j);
private:
    //! Bonds in the system
    std::unordered_set<bond, hash> _bonds;
    //! Angles in the system
    mutable std::unordered_set<angle, hash> _angles;
    //! Dihedral angles in the system
    mutable std::unordered_set<dihedral, hash> _dihedrals;
    //! Is the cached content up to date ?
    mutable bool uptodate;
};

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

    //! Get a reference to the atom at the position \c index
    Atom& operator[](size_t index) {return _templates[_atoms[index]];}
    //! Get a const (non-modifiable) reference to the atom at the position \c index
    const Atom& operator[](size_t index) const {return _templates[_atoms[index]];}

    //! Add an atom in the system
    void append(const Atom& _atom);
    //! Delete an atom in the system. If \c idx is out of bounds, do nothing.
    void remove(size_t idx);
    //! Add a bond in the system, between the atoms at index \c atom_i and \c atom_j
    void add_bond(size_t atom_i, size_t atom_j) {
        _connect.add_bond(atom_i, atom_j);
    }
    //! Remove a bond in the system, between the atoms at index \c atom_i and \c atom_j
    void remove_bond(size_t atom_i, size_t atom_j){
        _connect.remove_bond(atom_i, atom_j);
    }

    //! Get the number of atoms in the topology
    size_t natoms() const {return _atoms.size();}
    //! Get the number of atom types in the topology
    size_t natom_types() const {return _templates.size();}
    //! Reserve space for \c natoms in the topology
    void resize(size_t natoms) {_atoms.resize(natoms);}
    //! Clear the topology
    void clear();

    //! Try to guess the bonds in the system.
    void guess_bonds();

    //! Check wether the atoms at indexes \c i and \c j are bonded or not
    bool isbond(size_t i, size_t j);
    //! Check wether the atoms at indexes \c i, \c j and \c k constitues an angle
    bool isangle(size_t i, size_t j, size_t k);
    //! Check wether the atoms at indexes \c i \c j, \c k and \c m constitues a
    //! dihedral angle
    bool isdihedral(size_t i, size_t j, size_t k, size_t m);

    //! Get the bonds in the system
    std::vector<bond> bonds(void) const;
    //! Get the angles in the system
    std::vector<angle> angles(void) const;
    //! Get the dihedral angles in the system
    std::vector<dihedral> dihedrals(void) const;
private:
    //! Internal list of particle templates. If the same particle can be found
    //! more than one in a topology, the Atom class will have only one instance,
    //! pointing to this vector.
    std::vector<Atom> _templates;
    //! Internal list of atoms. The index refers to the _templates list
    std::vector<size_t> _atoms;
    //! Connectivity of the system. All the indices refers to the atoms in \c _atoms
    Connectivity _connect;
};

//! Create a topology containing \c natoms atoms, all of the UNDEFINED type.
Topology dummy_topology(size_t natoms);

} // namespace harp

#endif
