// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Error.hpp"
#include "chemfiles/Topology.hpp"

using namespace chemfiles;

void Topology::resize(size_t natoms) {
    for (auto& bond: connect_.bonds()) {
        if (bond[0] >= natoms || bond[1] >= natoms) {
            throw Error(
                "Can not resize the topology to contains " + std::to_string(natoms) +
                " atoms as there is a bond between atoms " + std::to_string(bond[0]) +
                "-" + std::to_string(bond[1]) + ".");
        }
    }
    atoms_.resize(natoms, Atom());
}

void Topology::append(Atom atom) {
    atoms_.emplace_back(std::move(atom));
}

void Topology::reserve(size_t natoms) {
    atoms_.reserve(natoms);
}

void Topology::add_bond(size_t atom_i, size_t atom_j) {
    connect_.add_bond(atom_i, atom_j);
}

void Topology::remove_bond(size_t atom_i, size_t atom_j) {
    connect_.remove_bond(atom_i, atom_j);
}


void Topology::remove(size_t idx) {
    assert(idx < natoms() && "Can not remove out of bounds atom");
    atoms_.erase(atoms_.begin() + static_cast<std::ptrdiff_t>(idx));
    auto bonds = connect_.bonds();
    for (auto& bond : bonds) {
        if (bond[0] == idx || bond[1] == idx) {
            connect_.remove_bond(bond[0], bond[1]);
        }
    }
}

const std::vector<Bond>& Topology::bonds() const {
    return connect_.bonds().as_vec();
}

const std::vector<Angle>& Topology::angles() const {
    return connect_.angles().as_vec();
}

const std::vector<Dihedral>& Topology::dihedrals() const {
    return connect_.dihedrals().as_vec();
}

bool Topology::isbond(size_t i, size_t j) const {
    if (i == j) {
        return false;
    }
    auto bonds = connect_.bonds();
    auto pos = bonds.find(Bond(i, j));
    return pos != bonds.end();
}

bool Topology::isangle(size_t i, size_t j, size_t k) const {
    if (i == j || j == k || i == k) {
        return false;
    }
    auto angles = connect_.angles();
    auto pos = angles.find(Angle(i, j, k));
    return pos != angles.end();
}

bool Topology::isdihedral(size_t i, size_t j, size_t k, size_t m) const {
    if (i == j || j == k || k == m || i == k || j == m || i == m) {
        return false;
    }
    auto dihedrals = connect_.dihedrals();
    auto pos = dihedrals.find(Dihedral(i, j, k, m));
    return pos != dihedrals.end();
}

void Topology::add_residue(Residue residue) {
    for (auto i: residue) {
        auto it = residue_mapping_.find(i);
        if (it != residue_mapping_.end()) {
            auto resid = residues_[it->second].id();
            throw Error(
                "Can not add this residue: atom " + std::to_string(i) +
                " is already in the residue " + std::to_string(resid)
            );
        }
    }
    auto resid = residues_.size();
    residues_.emplace_back(std::move(residue));
    for (auto i: residues_.back()) {
        residue_mapping_.insert({i, resid});
    }
}

bool Topology::are_linked(const Residue& first, const Residue& second) const {
    if (first == second) {
        return true;
    }
    for (auto i: first) {
        for (auto j: second) {
            if (isbond(i, j)) {
                return true;
            }
        }
    }
    return false;
}

optional<const Residue&> Topology::residue(size_t atom) const {
    auto it = residue_mapping_.find(atom);
    if (it == residue_mapping_.end()) {
        // This atom is not in a residue
        return nullopt;
    } else {
        return residues_[it->second];
    }
}
