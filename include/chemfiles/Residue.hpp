/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_RESIDUE_HPP
#define CHEMFILES_RESIDUE_HPP

#include <string>

#include "chemfiles/exports.hpp"
#include "chemfiles/sorted_set.hpp"

namespace chemfiles {

/*!
 * @class Residue Residue.hpp Residue.cpp
 * @brief A groupement of atoms in the same logical unit.
 *
 * A `Residue` is a group of atoms belonging to the same logical unit. They
 * can be small molecules, amino-acids in a protein, monomers in polymers,
 * etc.
 *
 * Iterating over a `Residue` gives the indexes of the atoms in the
 * associated `Topology`.
 */
class CHFL_EXPORT Residue final {
public:
    /// Create a new residue with a given `name` and residue id `resid`.
    explicit Residue(std::string name, uint64_t resid = static_cast<uint64_t>(-1));

    Residue(const Residue&) = default;
    Residue& operator=(const Residue&) = default;
    Residue(Residue&&) = default;
    Residue& operator=(Residue&&) = default;

    /// Get the name of the residue
    const std::string& name() const {
        return name_;
    }
    /// Get the index of the residue in the initial topology file. If the
    /// residue has no index, `static_cast<uint64_t>(-1)` is returned.
    uint64_t id() const {
        return id_;
    }
    /// Get the size of the residue, i.e. the number of atoms in this residue.
    size_t size() const {
        return atoms_.size();
    }

    /// Add an atom with index `i` to this residue
    void add_atom(size_t i);
    /// Check if the residue contains a given atom with index `i`
    bool contains(size_t i) const;

    using const_iterator = sorted_set<size_t>::const_iterator;
    // Iterators over the indexes of the atoms in the residue
    const_iterator begin() const {return atoms_.begin();}
    const_iterator end() const {return atoms_.end();}
    const_iterator cbegin() const {return atoms_.cbegin();}
    const_iterator cend() const {return atoms_.cend();}

private:
    /// Name of the residue
    std::string name_;
    /// Index of the residue in the initial topology file
    uint64_t id_ = static_cast<uint64_t>(-1);
    /// Indexes of the atoms in this residue. These indexes refers to the
    /// associated topology.
    sorted_set<size_t> atoms_;
};

} // namespace chemfiles

#endif
