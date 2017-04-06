// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Connectivity.hpp"

using namespace chemfiles;

static inline bool not_in_angle(const Angle& angle, size_t index) {
    return angle[0] != index && angle[1] != index && angle[2] != index;
}

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
            auto angle = Angle(
                static_cast<size_t>(-1), static_cast<size_t>(-2), static_cast<size_t>(-3)
            );
            if (bond1[0] == bond2[1]) {
                angle = Angle(bond2[0], bond2[1], bond1[1]);
                angles_.insert(angle);
            } else if (bond1[1] == bond2[0]) {
                angle = Angle(bond1[0], bond1[1], bond2[1]);
                angles_.insert(angle);
            } else if (bond1[1] == bond2[1]) {
                angle = Angle(bond1[0], bond1[1], bond2[0]);
                angles_.insert(angle);
            } else if (bond1[0] == bond2[0]) {
                angle = Angle(bond1[1], bond1[0], bond2[1]);
                angles_.insert(angle);
            } else {
                // We will not find any dihedral angle from these bonds
                continue;
            }
            // Find dihedral angles
            for (auto const& bond3 : bonds_) {
                if (bond2 == bond3 || bond1 == bond3) {
                    continue;
                }

                if (angle[0] == bond3[0] && not_in_angle(angle, bond3[1])) {
                    dihedrals_.emplace(bond3[1], angle[0], angle[1], angle[2]);
                } else if (angle[0] == bond3[1] && not_in_angle(angle, bond3[0])) {
                    dihedrals_.emplace(bond3[0], angle[0], angle[1], angle[2]);
                } else if (angle[2] == bond3[0] && not_in_angle(angle, bond3[1])) {
                    dihedrals_.emplace(angle[0], angle[1], angle[2], bond3[1]);
                } else if (angle[2] == bond3[1] && not_in_angle(angle, bond3[0])) {
                    dihedrals_.emplace(angle[0], angle[1], angle[2], bond3[0]);
                } else if (angle[1] == bond3[0] && not_in_angle(angle, bond3[1])) {
                    // TODO this is an improper dihedral
                } else if (angle[1] == bond3[1] && not_in_angle(angle, bond3[0])) {
                    // TODO this is an improper dihedral
                }
            }
        }
    }
    uptodate_ = true;
}

const sorted_set<Bond>& Connectivity::bonds() const {
    if (!uptodate_) {
        recalculate();
    }
    return bonds_;
}

const sorted_set<Angle>& Connectivity::angles() const {
    if (!uptodate_) {
        recalculate();
    }
    return angles_;
}

const sorted_set<Dihedral>& Connectivity::dihedrals() const {
    if (!uptodate_) {
        recalculate();
    }
    return dihedrals_;
}

void Connectivity::add_bond(size_t i, size_t j) {
    uptodate_ = false;
    bonds_.emplace(i, j);
}

void Connectivity::remove_bond(size_t i, size_t j) {
    auto pos = bonds_.find(Bond(i, j));
    if (pos != bonds_.end()) {
        uptodate_ = false;
        bonds_.erase(pos);
    }
}
