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

void Topology::add_atom(Atom atom) {
    atoms_.emplace_back(std::move(atom));
}

void Topology::reserve(size_t natoms) {
    atoms_.reserve(natoms);
}

void Topology::add_bond(size_t atom_i, size_t atom_j) {
    if (atom_i >= natoms() || atom_j >= natoms()) {
        throw OutOfBounds(
            "out of bounds atomic index in `Topology::add_bond`: we have " +
            std::to_string(natoms()) + " atoms, but the bond indexes are " +
            std::to_string(atom_i) + " and " + std::to_string(atom_j)
        );
    }
    connect_.add_bond(atom_i, atom_j);
}

void Topology::remove_bond(size_t atom_i, size_t atom_j) {
    if (atom_i >= natoms() || atom_j >= natoms()) {
        throw OutOfBounds(
            "out of bounds atomic index in `Topology::remove_bond`: we have " +
            std::to_string(natoms()) + " atoms, but the bond indexes are " +
            std::to_string(atom_i) + " and " + std::to_string(atom_j)
        );
    }
    connect_.remove_bond(atom_i, atom_j);
}


void Topology::remove(size_t i) {
    if (i >= natoms()) {
        throw OutOfBounds(
            "out of bounds atomic index in `Topology::remove`: we have " +
            std::to_string(natoms()) + " atoms, but the index is " + std::to_string(i)
        );
    }
    atoms_.erase(atoms_.begin() + static_cast<std::ptrdiff_t>(i));
    auto bonds = connect_.bonds();
    for (auto& bond : bonds) {
        if (bond[0] == i || bond[1] == i) {
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
            if (is_bond(i, j)) {
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
