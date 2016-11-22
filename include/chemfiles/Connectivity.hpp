/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_CONNECTIVITY_HPP
#define CHEMFILES_CONNECTIVITY_HPP

#include <array>
#include <cassert>
#include <unordered_set>

#include "chemfiles/exports.hpp"
#include "chemfiles/Error.hpp"

namespace chemfiles {

//! The bond struct ensure a canonical representation of a bond between atoms
//! i and j, with i<j
struct CHFL_EXPORT Bond {
    Bond(size_t first, size_t second) {
        if (first == second) {
            throw Error("Can not have a bond between an atom and itself");
        }

        data_[0] = std::min(first, second);
        data_[1] = std::max(first, second);
    }

    //! Indexing operator
    const size_t& operator[](size_t i) const { return data_[i]; }
    //! Comparison operator
    bool operator==(const Bond& other) const {
        return data_[0] == other[0] && data_[1] == other[1];
    }

    Bond(Bond&&) = default;
    Bond& operator=(Bond&&) = default;
    Bond(const Bond&) = default;
    Bond& operator=(const Bond&) = default;

private:
    std::array<size_t, 2> data_;
};

//! The angle struct ensure a canonical representation of an angle between the
//! atoms i, j and k, with i < k
struct CHFL_EXPORT Angle {
    Angle(size_t first, size_t midle, size_t last) {
        if (first == midle || first == last || midle == last) {
            throw Error("Can not have the same atom twice in an angle");
        }

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

    Angle(Angle&&) = default;
    Angle& operator=(Angle&&) = default;
    Angle(const Angle&) = default;
    Angle& operator=(const Angle&) = default;

private:
    std::array<size_t, 3> data_;
};

//! The dihedral struct ensure a canonical representation of a dihedral angle
//! between the atoms i, j, k and m, with max(i, j) < max(k, m))
struct CHFL_EXPORT Dihedral {
    Dihedral(size_t first, size_t second, size_t third, size_t fourth) {
        if (first == second || second == third || third == fourth) {
            throw Error("Can not have an atom linked to itself in a dihedral angle");
        }

        if (first == third || second == fourth || first == fourth) {
            throw Error("Can not have an atom twice in a dihedral angle");
        }

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

    Dihedral(Dihedral&&) = default;
    Dihedral& operator=(Dihedral&&) = default;
    Dihedral(const Dihedral&) = default;
    Dihedral& operator=(const Dihedral&) = default;

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


} // namespace chemfiles

#endif
