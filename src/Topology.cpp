/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <algorithm>
#include <cstddef>

#include "chemfiles/Error.hpp"
#include "chemfiles/Topology.hpp"

using namespace chemfiles;

void Connectivity::recalculate() const {
    angles_.clear();
    dihedrals_.clear();
    for (auto const& bond1 : bonds_) {
        // Find angles
        for (auto const& bond2 : bonds_) {
            if (bond1 == bond2)
                continue;
            // Initializing angle to an invalid value
            Angle angle1(static_cast<size_t>(-1), static_cast<size_t>(-2),
                         static_cast<size_t>(-3));
            if (bond1[0] == bond2[1]) {
                angle1 = Angle(bond2[0], bond2[1], bond1[1]);
                angles_.insert(angle1);
            } else if (bond1[1] == bond2[0]) {
                angle1 = Angle(bond1[0], bond1[1], bond2[1]);
                angles_.insert(angle1);
            } else if (bond1[1] == bond2[1]) {
                angle1 = Angle(bond1[0], bond1[1], bond2[0]);
                angles_.insert(angle1);
            } else if (bond1[0] == bond2[0]) {
                angle1 = Angle(bond1[1], bond1[0], bond2[1]);
                angles_.insert(angle1);
            } else {
                // We will not find any dihedral angle from these bonds
                continue;
            }
            // Find dihedral angles
            for (auto const& bond3 : bonds_) {
                if (bond2 == bond3)
                    continue;

                if (angle1[2] == bond3[0] && angle1[1] != bond3[1]) {
                    dihedrals_.emplace(angle1[0], angle1[1], angle1[2],
                                       bond3[1]);
                } else if (angle1[0] == bond3[1] && angle1[1] != bond3[0]) {
                    dihedrals_.emplace(bond3[0], angle1[0], angle1[1],
                                       angle1[2]);
                } else if (angle1[2] == bond3[0] || angle1[2] == bond3[1]) {
                    // TODO this is an improper dihedral
                }
            }
        }
    }
    uptodate = true;
}

const std::unordered_set<Bond>& Connectivity::bonds() const {
    if (!uptodate)
        recalculate();
    return bonds_;
}

const std::unordered_set<Angle>& Connectivity::angles() const {
    if (!uptodate)
        recalculate();
    return angles_;
}

const std::unordered_set<Dihedral>& Connectivity::dihedrals() const {
    if (!uptodate)
        recalculate();
    return dihedrals_;
}

void Connectivity::add_bond(size_t i, size_t j) {
    uptodate = false;
    bonds_.emplace(i, j);
}

void Connectivity::remove_bond(size_t i, size_t j) {
    auto pos = bonds_.find(Bond(i, j));
    if (pos != bonds_.end()) {
        uptodate = false;
        bonds_.erase(pos);
    }
}

/******************************************************************************/

void Topology::resize(size_t size) {
    for (auto bond : bonds()) {
        if (bond[0] >= size || bond[1] >= size) {
            throw APIError(
                "Can not resize the topology to " + std::to_string(size) +
                " as there is a bond between atoms " + std::to_string(bond[0]) +
                "-" + std::to_string(bond[1]) + ".");
        }
    }
    atoms_.resize(size, Atom(Atom::UNDEFINED));
}

void Topology::append(const Atom& atom) {
    atoms_.push_back(atom);
}

void Topology::remove(size_t idx) {
    atoms_.erase(begin(atoms_) + static_cast<ptrdiff_t>(idx));
    auto bonds = connect_.bonds();
    for (auto& bond : bonds) {
        if (bond[0] == idx || bond[1] == idx)
            connect_.remove_bond(bond[0], bond[1]);
    }
}

std::vector<Bond> Topology::bonds() const {
    std::vector<Bond> res;
    res.insert(begin(res), begin(connect_.bonds()), end(connect_.bonds()));
    return res;
}

std::vector<Angle> Topology::angles() const {
    std::vector<Angle> res;
    res.insert(begin(res), begin(connect_.angles()), end(connect_.angles()));
    return res;
}

std::vector<Dihedral> Topology::dihedrals() const {
    std::vector<Dihedral> res;
    res.insert(begin(res), begin(connect_.dihedrals()),
               end(connect_.dihedrals()));
    return res;
}

bool Topology::isbond(size_t i, size_t j) const {
    auto bonds = connect_.bonds();
    auto pos = bonds.find(Bond(i, j));
    return pos != end(bonds);
}

bool Topology::isangle(size_t i, size_t j, size_t k) const {
    auto angles = connect_.angles();
    auto pos = angles.find(Angle(i, j, k));
    return pos != end(angles);
}

bool Topology::isdihedral(size_t i, size_t j, size_t k, size_t m) const {
    auto dihedrals = connect_.dihedrals();
    auto pos = dihedrals.find(Dihedral(i, j, k, m));
    return pos != end(dihedrals);
}

Topology chemfiles::dummy_topology(size_t natoms) {
    Topology topology;
    for (size_t i = 0; i < natoms; i++)
        topology.append(Atom(Atom::UNDEFINED));
    return topology;
}
