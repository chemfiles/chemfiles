// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Connectivity.hpp"

using namespace chemfiles;

void Connectivity::recalculate() const {
    angles_.clear();
    dihedrals_.clear();

    // Generate the list of which atom is bonded to which one
    auto bonded_to = std::vector<std::vector<size_t>>(biggest_atom_ + 1);
    for (auto const& bond: bonds_) {
        assert(bond[0] < bonded_to.size());
        assert(bond[1] < bonded_to.size());
        bonded_to[bond[0]].push_back(bond[1]);
        bonded_to[bond[1]].push_back(bond[0]);
    }

    // Generate the list of angles
    for (auto const& bond: bonds_) {
        auto i = bond[0];
        auto j = bond[1];
        for (auto k: bonded_to[i]) {
            if (k != j) {
                angles_.insert(Angle(k, i, j));
            }
        }

        for (auto k: bonded_to[j]) {
            if (k != i) {
                angles_.insert(Angle(i, j, k));
            }
        }
    }

    // Generate the list of dihedrals
    for (auto const& angle: angles_) {
        auto i = angle[0];
        auto j = angle[1];
        auto k = angle[2];
        for (auto m: bonded_to[i]) {
            if (m != j && m != k) {
                dihedrals_.insert(Dihedral(m, i, j, k));
            }
        }

        for (auto m: bonded_to[k]) {
            if (m != i && m != j) {
                dihedrals_.insert(Dihedral(i, j, k, m));
            }
        }
    }

    // TODO: generate the impropers

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
    if (i > biggest_atom_) {biggest_atom_ = i;}
    if (j > biggest_atom_) {biggest_atom_ = j;}
}

void Connectivity::remove_bond(size_t i, size_t j) {
    auto pos = bonds_.find(Bond(i, j));
    if (pos != bonds_.end()) {
        uptodate_ = false;
        bonds_.erase(pos);
    }
}
