/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Topology.hpp"
#include <algorithm>

using namespace harp;
using std::vector;

void Connectivity::recalculate() const{
    for (auto const& b1 : _bonds){
        // Find angles
        for (auto const& b2 : _bonds){
            if (b1 == b2) continue;
            // Initializing angle to an invalid value
            angle a1(static_cast<size_t>(-1), static_cast<size_t>(-2), static_cast<size_t>(-3));
            if (b1[0] == b2[1]) {
                a1 = angle(b2[0], b2[1], b1[1]);
                _angles.insert(a1);
            } else if (b1[1] == b2[0]) {
                a1 = angle(b1[0], b1[1], b2[1]);
                _angles.insert(a1);
            } else {
                // We will not find any dihedral angle from these bonds
                continue;
            }
            // Find dihedral angles
            for (auto const& b3 : _bonds){
                if (b2 == b3) continue;

                if (a1[2] == b3[0]){
                    _dihedrals.insert(dihedral(a1[0], a1[1], a1[2], b3[1]));
                } else if (a1[0] == b3[1]) {
                    _dihedrals.insert(dihedral(b3[0], a1[0], a1[1], a1[2]));
                } else if (a1[2] == b3[0] || a1[2] == b3[1]) {
                    // TODO this is an improper dihedral
                }
            }
        }
    }
    uptodate = true;
}

void Connectivity::clear(){
    _bonds.clear();
    _angles.clear();
    _dihedrals.clear();
}

const std::unordered_set<angle, hash>& Connectivity::angles() const {
    if (not uptodate)
        recalculate();
    return _angles;
}

const std::unordered_set<dihedral, hash>& Connectivity::dihedrals() const {
    if (not uptodate)
        recalculate();
    return _dihedrals;
}

void Connectivity::add_bond(size_t i, size_t j){
    uptodate = false;
    _bonds.insert(bond(i, j));
}

void Connectivity::remove_bond(size_t i, size_t j){
    uptodate = false;
    auto pos = _bonds.find(bond(i, j));
    if (pos != _bonds.end()){
        _bonds.erase(pos);
    }
}

/******************************************************************************/

Topology::Topology(size_t natoms) {
    resize(natoms);
}

Topology::Topology() : Topology(100) {}

void Topology::append(const Atom& _atom){
    size_t index = static_cast<size_t>(-1);

    for (size_t i = 0 ; i<_templates.size(); i++)
        if (_templates[i] == _atom)
            index = i;
    if (index == static_cast<size_t>(-1)) { // Atom not found
        _templates.push_back(_atom);
        index = _templates.size() - 1;
    }

    _atoms.push_back(index);
}

void Topology::remove(size_t idx) {
    if (idx < _atoms.size())
        _atoms.erase(begin(_atoms) + idx);
    auto bonds = _connect.bonds();
    for (auto& bond : bonds){
        if (bond[0] == idx || bond[1] == idx)
            _connect.remove_bond(bond[0], bond[1]);
    }
}

vector<bond> Topology::bonds(void)  const{
    vector<bond> res;
    res.insert(begin(res), begin(_connect.bonds()), end(_connect.bonds()));
    return std::move(res);
}

vector<angle> Topology::angles(void) const{
    vector<angle> res;
    res.insert(begin(res), begin(_connect.angles()), end(_connect.angles()));
    return std::move(res);
}

vector<dihedral> Topology::dihedrals(void) const{
    vector<dihedral> res;
    res.insert(begin(res), begin(_connect.dihedrals()), end(_connect.dihedrals()));
    return std::move(res);
}

bool Topology::isbond(size_t i, size_t j) {
    auto bonds = _connect.bonds();
    auto pos = bonds.find(bond(i, j));
    return pos != end(bonds);
}

bool Topology::isangle(size_t i, size_t j, size_t k) {
    auto angles = _connect.angles();
    auto pos = angles.find(angle(i, j, k));
    return pos != end(angles);
}

bool Topology::isdihedral(size_t i, size_t j, size_t k, size_t m) {
    auto dihedrals = _connect.dihedrals();
    auto pos = dihedrals.find(dihedral(i, j, k, m));
    return pos != end(dihedrals);
}

void Topology::clear(){
    _templates.clear();
    _atoms.clear();
    _connect.clear();
}

void Topology::guess_bonds(){
    //! TODO
}

Topology harp::dummy_topology(size_t natoms){
    Topology top(natoms);
    for (size_t i=0; i<natoms; i++)
        top.append(Atom(Atom::UNDEFINED));
    return top;
}
