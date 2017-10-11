// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CONNECTIVITY_HPP
#define CHEMFILES_CONNECTIVITY_HPP

#include <array>
#include <cassert>

#include "chemfiles/sorted_set.hpp"
#include "chemfiles/exports.hpp"

namespace chemfiles {

/// The `Bond` struct ensure a canonical representation of a bond between atoms
/// i and j, with i<j
struct CHFL_EXPORT Bond {
    Bond(size_t i, size_t j);
    Bond(Bond&&) = default;
    Bond& operator=(Bond&&) = default;
    Bond(const Bond&) = default;
    Bond& operator=(const Bond&) = default;

    /// Get the index of the `i`th atom (`i == 0` or `i == 1`) in the bond.
    ///
    /// @throws OutOfBounds if `i` is not 0 or 1
    size_t operator[](size_t i) const;

private:
    std::array<size_t, 2> data_;

    friend bool operator==(const Bond&, const Bond&);
    friend bool operator!=(const Bond&, const Bond&);
    friend bool operator<(const Bond&, const Bond&);
    friend bool operator<=(const Bond&, const Bond&);
    friend bool operator>(const Bond&, const Bond&);
    friend bool operator>=(const Bond&, const Bond&);
};

inline bool operator==(const Bond& lhs, const Bond& rhs) {
    return lhs.data_ == rhs.data_;
}

inline bool operator!=(const Bond& lhs, const Bond& rhs) {
    return lhs.data_ != rhs.data_;
}

inline bool operator<(const Bond& lhs, const Bond& rhs) {
    return lhs.data_ < rhs.data_;
}

inline bool operator<=(const Bond& lhs, const Bond& rhs) {
    return lhs.data_ <= rhs.data_;
}

inline bool operator>(const Bond& lhs, const Bond& rhs) {
    return lhs.data_ > rhs.data_;
}

inline bool operator>=(const Bond& lhs, const Bond& rhs) {
    return lhs.data_ >= rhs.data_;
}

/// The `Angle` struct ensure a canonical representation of an angle between the
/// atoms i, j and k, with i < k
struct CHFL_EXPORT Angle {
    Angle(size_t i, size_t j, size_t k);
    Angle(Angle&&) = default;
    Angle& operator=(Angle&&) = default;
    Angle(const Angle&) = default;
    Angle& operator=(const Angle&) = default;

    /// Get the index of the `i`th atom (`i == 0`, `i == 1` or `i == 2`) in the
    /// angle.
    ///
    /// @throws OutOfBounds if `i` is not 0, 1 or 2
    size_t operator[](size_t i) const;

private:
    std::array<size_t, 3> data_;

    friend bool operator==(const Angle&, const Angle&);
    friend bool operator!=(const Angle&, const Angle&);
    friend bool operator<(const Angle&, const Angle&);
    friend bool operator<=(const Angle&, const Angle&);
    friend bool operator>(const Angle&, const Angle&);
    friend bool operator>=(const Angle&, const Angle&);
};

inline bool operator==(const Angle& lhs, const Angle& rhs) {
    return lhs.data_ == rhs.data_;
}

inline bool operator!=(const Angle& lhs, const Angle& rhs) {
    return lhs.data_ != rhs.data_;
}

inline bool operator<(const Angle& lhs, const Angle& rhs) {
    return lhs.data_ < rhs.data_;
}

inline bool operator<=(const Angle& lhs, const Angle& rhs) {
    return lhs.data_ < rhs.data_;
}

inline bool operator>(const Angle& lhs, const Angle& rhs) {
    return lhs.data_ > rhs.data_;
}

inline bool operator>=(const Angle& lhs, const Angle& rhs) {
    return lhs.data_ > rhs.data_;
}

/// The `Dihedral` struct ensure a canonical representation of a dihedral angle
/// between the atoms i, j, k and m, with max(i, j) < max(k, m))
struct CHFL_EXPORT Dihedral {
    Dihedral(size_t i, size_t j, size_t k, size_t m);
    Dihedral(Dihedral&&) = default;
    Dihedral& operator=(Dihedral&&) = default;
    Dihedral(const Dihedral&) = default;
    Dihedral& operator=(const Dihedral&) = default;

    /// Get the index of the `i`th atom (`i` can be 0, 1, 2 or 3) in the
    /// dihedral.
    ///
    /// @throws OutOfBounds if `i` is not 0, 1, 2 or 3.
    size_t operator[](size_t i) const;

private:
    std::array<size_t, 4> data_;

    friend bool operator==(const Dihedral&, const Dihedral&);
    friend bool operator!=(const Dihedral&, const Dihedral&);
    friend bool operator<(const Dihedral&, const Dihedral&);
    friend bool operator<=(const Dihedral&, const Dihedral&);
    friend bool operator>(const Dihedral&, const Dihedral&);
    friend bool operator>=(const Dihedral&, const Dihedral&);
};

inline bool operator==(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs.data_ == rhs.data_;
}

inline bool operator!=(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs.data_ != rhs.data_;
}

inline bool operator<(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs.data_ < rhs.data_;
}

inline bool operator<=(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs.data_ <= rhs.data_;
}

inline bool operator>(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs.data_ > rhs.data_;
}

inline bool operator>=(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs.data_ >= rhs.data_;
}

/// The `Improper` struct ensure a canonical representation of an improper
/// dihedral angle centered around the atom j, which is linked to i, k, and m.
/// i, k, and m are sorted to ensure the canonical representation
struct CHFL_EXPORT Improper {
    Improper(size_t i, size_t j, size_t k, size_t m);
    Improper(Improper&&) = default;
    Improper& operator=(Improper&&) = default;
    Improper(const Improper&) = default;
    Improper& operator=(const Improper&) = default;

    /// Get the index of the `i`th atom (`i` can be 0, 1, 2 or 3) in the
    /// improper.
    ///
    /// @throws OutOfBounds if `i` is not 0, 1, 2 or 3.
    size_t operator[](size_t i) const;

private:
    std::array<size_t, 4> data_;

    friend bool operator==(const Improper&, const Improper&);
    friend bool operator!=(const Improper&, const Improper&);
    friend bool operator<(const Improper&, const Improper&);
    friend bool operator<=(const Improper&, const Improper&);
    friend bool operator>(const Improper&, const Improper&);
    friend bool operator>=(const Improper&, const Improper&);
};

inline bool operator==(const Improper& lhs, const Improper& rhs) {
    return lhs.data_ == rhs.data_;
}

inline bool operator!=(const Improper& lhs, const Improper& rhs) {
    return lhs.data_ != rhs.data_;
}

inline bool operator<(const Improper& lhs, const Improper& rhs) {
    return lhs.data_ < rhs.data_;
}

inline bool operator<=(const Improper& lhs, const Improper& rhs) {
    return lhs.data_ <= rhs.data_;
}

inline bool operator>(const Improper& lhs, const Improper& rhs) {
    return lhs.data_ > rhs.data_;
}

inline bool operator>=(const Improper& lhs, const Improper& rhs) {
    return lhs.data_ >= rhs.data_;
}

/// The connectivity struct store a cache of the bonds, angles and dihedrals
/// in the system. The `recalculate` function should be called when bonds are
/// added or removed. The `bonds` set is the main source of information, all the
/// other data are cached from it.
class Connectivity {
public:
    Connectivity() = default;
    /// Recalculate the angles and the dihedrals from the bond list
    void recalculate() const;
    /// Get the bonds in this connectivity
    const sorted_set<Bond>& bonds() const;
    /// Get the angles in this connectivity
    const sorted_set<Angle>& angles() const;
    /// Get the dihedrals in this connectivity
    const sorted_set<Dihedral>& dihedrals() const;
    /// Get the impropers in this connectivity
    const sorted_set<Improper>& impropers() const;
    /// Add a bond between the atoms `i` and `j`
    void add_bond(size_t i, size_t j);
    /// Remove any bond between the atoms `i` and `j`
    void remove_bond(size_t i, size_t j);

private:
    /// Biggest index within the atoms we know about. Used to pre-allocate
    /// memory when recomputing bonds.
    size_t biggest_atom_ = 0;
    /// Bonds in the system
    sorted_set<Bond> bonds_;
    /// Angles in the system
    mutable sorted_set<Angle> angles_;
    /// Dihedral angles in the system
    mutable sorted_set<Dihedral> dihedrals_;
    /// Improper dihedral angles in the system
    mutable sorted_set<Improper> impropers_;
    /// Is the cached content up to date ?
    mutable bool uptodate_ = false;
};

} // namespace chemfiles

#endif
