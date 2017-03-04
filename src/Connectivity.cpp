// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/Connectivity.hpp"

using namespace chemfiles;

void Connectivity::recalculate() const {
    angles_.clear();
    dihedrals_.clear();
    for (auto const& bond1 : bonds_) {
        // Find angles
        for (auto const& bond2 : bonds_) {
            if (bond1 == bond2) {
                continue;
            }
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
                if (bond2 == bond3) {
                    continue;
                }

                if (angle1[0] == bond3[0] && angle1[1] != bond3[1]) {
                    dihedrals_.emplace(bond3[1], angle1[0], angle1[1], angle1[2]);
                } else if (angle1[0] == bond3[1] && angle1[1] != bond3[0]) {
                    dihedrals_.emplace(bond3[0], angle1[0], angle1[1], angle1[2]);
                } else if (angle1[2] == bond3[0] && angle1[1] != bond3[1]) {
                    dihedrals_.emplace(angle1[0], angle1[1], angle1[2], bond3[1]);
                } else if (angle1[2] == bond3[1] && angle1[1] != bond3[0]) {
                    dihedrals_.emplace(angle1[0], angle1[1], angle1[2], bond3[0]);
                } else if (angle1[1] == bond3[0] && angle1[0] != bond3[1] && angle1[2] != bond3[1]) {
                    // TODO this is an improper dihedral
                } else if (angle1[1] == bond3[1] && angle1[0] != bond3[0] && angle1[2] != bond3[0]) {
                    // TODO this is an improper dihedral
                }
            }
        }
    }
    uptodate = true;
}

const sorted_set<Bond>& Connectivity::bonds() const {
    if (!uptodate) {
        recalculate();
    }
    return bonds_;
}

const sorted_set<Angle>& Connectivity::angles() const {
    if (!uptodate) {
        recalculate();
    }
    return angles_;
}

const sorted_set<Dihedral>& Connectivity::dihedrals() const {
    if (!uptodate) {
        recalculate();
    }
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
