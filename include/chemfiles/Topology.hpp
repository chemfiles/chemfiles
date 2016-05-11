/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_TOPOLOGY_HPP
#define CHEMFILES_TOPOLOGY_HPP

#include <array>
#include <cassert>
#include <functional>
#include <unordered_set>
#include <vector>

#include "chemfiles/Atom.hpp"
#include "chemfiles/exports.hpp"

namespace chemfiles {

//! The bond struct ensure a canonical representation of a bond between atoms
//! i and j, with i<j
struct CHFL_EXPORT Bond {
    Bond(size_t first, size_t second) {
        assert(first != second);
        data_[0] = std::min(first, second);
        data_[1] = std::max(first, second);
    }
    //! Indexing operator
    const size_t& operator[](size_t i) const { return data_[i]; }
    //! Comparison operator
    bool operator==(const Bond& other) const {
        return data_[0] == other[0] && data_[1] == other[1];
    }

private:
    std::array<size_t, 2> data_;
};

//! The angle struct ensure a canonical representation of an angle between the
//! atoms i, j and k, with i < k
struct CHFL_EXPORT Angle {
    Angle(size_t first, size_t midle, size_t last) {
        assert(first != last);
        assert(first != midle);
        data_[0] = std::min(first, last);
        data_[1] = midle;
        data_[2] = std::max(first, last);
    }
    //! Indexing operator
    const size_t& operator[](size_t i) const { return data_[i]; }
    //! Comparison operator
    bool operator==(const Angle& other) const {
        return data_[0] == other[0] && data_[1] == other[1] &&
               data_[2] == other[2];
    }

private:
    std::array<size_t, 3> data_;
};

//! The dihedral struct ensure a canonical representation of a dihedral angle
//! between the atoms i, j, k and m, with max(i, j) < max(k, m))
struct CHFL_EXPORT Dihedral {
    Dihedral(size_t first, size_t second, size_t third, size_t fourth) {
        assert(first != second);
        assert(second != third);
        assert(third != fourth);

        if (std::max(first, second) < std::max(third, fourth)) {
            data_[0] = first;
            data_[1] = second;
            data_[2] = third;
            data_[3] = fourth;
        } else {
            data_[0] = fourth;
            data_[1] = third;
            data_[2] = second;
            data_[3] = first;
        }
    }
    //! Indexing operator
    const size_t& operator[](size_t i) const { return data_[i]; }
    //! Comparison operator
    bool operator==(const Dihedral& other) const {
        return data_[0] == other[0] && data_[1] == other[1] &&
               data_[2] == other[2] && data_[3] == other[3];
    }

private:
    std::array<size_t, 4> data_;
};

} // namespace chemfiles

namespace std {
    // We need a hashing function for the std::unordered_set, but it will not
    // be used. We will use the operator== instead to ensure the element
    // unicity in the sets.
    template <> struct hash<chemfiles::Bond> {
        size_t operator()(chemfiles::Bond const&) const { return 42; }
    };
    template <> struct hash<chemfiles::Angle> {
        size_t operator()(chemfiles::Angle const&) const { return 42; }
    };
    template <> struct hash<chemfiles::Dihedral> {
        size_t operator()(chemfiles::Dihedral const&) const { return 42; }
    };
} // namespace std

namespace chemfiles {

/*!
 * @class connectivity Topology.hpp Topology.cpp
 *
 * The connectivity struct store a cache of the bonds, angles and dihedrals
 * in the system. The `recalculate` function should be called when bonds are
 * added or removed. The `bonds` set is the main source of information, all the
 * other data are cached from it.
 */
class Connectivity {
public:
    Connectivity() = default;
    //! Recalculate the angles and the dihedrals from the bond list
    void recalculate() const;
    //! Access the underlying data
    const std::unordered_set<Bond>& bonds() const;
    const std::unordered_set<Angle>& angles() const;
    const std::unordered_set<Dihedral>& dihedrals() const;
    //! Add a bond between the atoms `i` and `j`
    void add_bond(size_t i, size_t j);
    //! Remove any bond between the atoms `i` and `j`
    void remove_bond(size_t i, size_t j);

private:
    //! Bonds in the system
    std::unordered_set<Bond> bonds_;
    //! Angles in the system
    mutable std::unordered_set<Angle> angles_;
    //! Dihedral angles in the system
    mutable std::unordered_set<Dihedral> dihedrals_;
    //! Is the cached content up to date ?
    mutable bool uptodate = false;
};

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
    void append(const Atom& _atom);
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

//! Create a topology containing `natoms` atoms, all of the UNDEFINED type.
CHFL_EXPORT Topology dummy_topology(size_t natoms);

} // namespace chemfiles

#endif
