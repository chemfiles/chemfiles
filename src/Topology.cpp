/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Topology.hpp"
#include <cassert>
#include <algorithm>

using namespace harp;

Topology::Topology(size_t natoms) {
    reserve(natoms);
}

Topology::Topology() : Topology(100) {}

Atom& Topology::operator[](size_t index) {
    assert(index < natoms());
    return _atom_types[_atoms[index]];
}

void Topology::append(Atom atom){
    size_t index = static_cast<size_t>(-1);
    for (size_t i = 0 ; i<_atoms.size(); i++)
        if (_atom_types[i] == atom)
            index = i;
    if (index == static_cast<size_t>(-1)) { // Atom not found
        _atom_types.push_back(atom);
        index = _atom_types.size() - 1;
    }

    _atoms.push_back(index);
    _bonds.push_back(vector<size_t>());
}

void Topology::add_bond(size_t atom_i, size_t atom_j){
    assert(atom_i < natoms());
    assert(atom_j < natoms());
    if (atom_i == atom_j ) return;
    _bonds[atom_i].push_back(atom_j);
    _bonds[atom_j].push_back(atom_i);
}

vector<bond> Topology::bonds(void){
    vector<bond> res;
    res.reserve(2*natoms());
    for (size_t i=0; i<natoms(); i++)
        for (auto other : _bonds[i])
            res.push_back(bond(i, other));
    res.shrink_to_fit();
    return std::move(res);
}

vector<angle> Topology::angles(void){
    vector<angle> res;
    res.reserve(3*natoms());
    for (size_t i=0; i<natoms(); i++)
        for (auto j : _bonds[i])
            for (auto k : _bonds[j])
                if (k != i) res.push_back(angle(i, j, k));
    res.shrink_to_fit();
    return std::move(res);
}

vector<dihedral> Topology::dihedrals(void){
    vector<dihedral> res;
    res.reserve(4*natoms());
    for (size_t i=0; i<natoms(); i++)
        for (auto j : _bonds[i])
            for (auto k : _bonds[j])
                if (k != i)
                    for (auto m : _bonds[k])
                        if (m != j) res.push_back(dihedral(i, j, k, m));
    res.shrink_to_fit();
    return std::move(res);
}

void Topology::clear(){
    _atom_types.clear();
    _atoms.clear();
    _bonds.clear();
}

Topology harp::dummy_topology(size_t natoms){
    Topology top(natoms);
    for (size_t i=0; i<natoms; i++)
        top.append(Atom(Atom::UNDEFINED));
    return top;
}
