// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/Topology.hpp"

using namespace chemfiles;

void Topology::resize(size_t size) {
    for (auto& bond: connect_.bonds()) {
        if (bond[0] >= size || bond[1] >= size) {
            throw error(
                "can not resize the topology to contains {} atoms as there "
                "is a bond between atoms {} - {}",
                size, bond[0], bond[1]
            );
        }
    }
    atoms_.resize(size, Atom());
}

void Topology::add_atom(Atom atom) {
    atoms_.emplace_back(std::move(atom));
}

void Topology::reserve(size_t size) {
    atoms_.reserve(size);
}

void Topology::add_bond(size_t atom_i, size_t atom_j) {
    if (atom_i >= size() || atom_j >= size()) {
        throw out_of_bounds(
            "out of bounds atomic index in `Topology::add_bond`: "
            "we have {} atoms, but the bond indexes are {} and {}",
            size(), atom_i, atom_j
        );
    }
    connect_.add_bond(atom_i, atom_j);
}

void Topology::remove_bond(size_t atom_i, size_t atom_j) {
    if (atom_i >= size() || atom_j >= size()) {
        throw out_of_bounds(
            "out of bounds atomic index in `Topology::remove_bond`: "
            "we have {} atoms, but the bond indexes are {} and {}",
            size(), atom_i, atom_j
        );
    }
    connect_.remove_bond(atom_i, atom_j);
}


void Topology::remove(size_t i) {
    if (i >= size()) {
        throw out_of_bounds(
            "out of bounds atomic index in `Topology::remove`: we have {} atoms, "
            "but the indexe is {}",
            size(), i
        );
    }
    atoms_.erase(atoms_.begin() + static_cast<std::ptrdiff_t>(i));
    // Remove all bonds with the removed atom
    auto bonds = connect_.bonds();
    for (auto& bond : bonds) {
        if (bond[0] == i || bond[1] == i) {
            connect_.remove_bond(bond[0], bond[1]);
        }
    }
    // Shift all bonds indexes
    connect_.atom_removed(i);
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

const std::vector<Improper>& Topology::impropers() const {
    return connect_.impropers().as_vec();
}

bool Topology::is_bond(size_t i, size_t j) const {
    if (i == j) {
        return false;
    }
    auto bonds = connect_.bonds();
    return bonds.find(Bond(i, j)) != bonds.end();
}

void Topology::add_residue(Residue residue) {
    for (auto i: residue) {
        auto it = residue_mapping_.find(i);
        if (it != residue_mapping_.end()) {
            throw error(
                "can not add this residue: atom {} is already in another residue",
                i
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
            if (is_bond(i, j)) {
                return true;
            }
        }
    }
    return false;
}

optional<const Residue&> Topology::residue_for_atom(size_t index) const {
    auto it = residue_mapping_.find(index);
    if (it == residue_mapping_.end()) {
        // This atom is not in a residue
        return nullopt;
    } else {
        return residues_[it->second];
    }
}
