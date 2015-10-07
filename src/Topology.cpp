/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <algorithm>
#include <cstddef>

#include "chemfiles/Topology.hpp"
#include "chemfiles/Error.hpp"

using namespace chemfiles;
using std::vector;

void Connectivity::recalculate() const{
    _angles.clear();
    _dihedrals.clear();
    for (auto const& bond1 : _bonds){
        // Find angles
        for (auto const& bond2 : _bonds){
            if (bond1 == bond2) continue;
            // Initializing angle to an invalid value
            angle angle1(static_cast<size_t>(-1), static_cast<size_t>(-2), static_cast<size_t>(-3));
            if (bond1[0] == bond2[1]) {
                angle1 = angle(bond2[0], bond2[1], bond1[1]);
                _angles.insert(angle1);
            } else if (bond1[1] == bond2[0]) {
                angle1 = angle(bond1[0], bond1[1], bond2[1]);
                _angles.insert(angle1);
            } else if (bond1[1] == bond2[1]) {
                angle1 = angle(bond1[0], bond1[1], bond2[0]);
                _angles.insert(angle1);
            } else if (bond1[0] == bond2[0]) {
                angle1 = angle(bond1[1], bond1[0], bond2[1]);
                _angles.insert(angle1);
            } else {
                // We will not find any dihedral angle from these bonds
                continue;
            }
            // Find dihedral angles
            for (auto const& bond3 : _bonds){
                if (bond2 == bond3) continue;

                if (angle1[2] == bond3[0] && angle1[1] != bond3[1]){
                    _dihedrals.insert(dihedral(angle1[0], angle1[1], angle1[2], bond3[1]));
                } else if (angle1[0] == bond3[1] && angle1[1] != bond3[0]) {
                    _dihedrals.insert(dihedral(bond3[0], angle1[0], angle1[1], angle1[2]));
                } else if (angle1[2] == bond3[0] || angle1[2] == bond3[1]) {
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

const std::unordered_set<bond>& Connectivity::bonds() const {
    if (!uptodate)
        recalculate();
    return _bonds;
}

const std::unordered_set<angle>& Connectivity::angles() const {
    if (!uptodate)
        recalculate();
    return _angles;
}

const std::unordered_set<dihedral>& Connectivity::dihedrals() const {
    if (!uptodate)
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

Topology::Topology() : Topology(0) {}

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
        _atoms.erase(begin(_atoms) + static_cast<ptrdiff_t>(idx));
    auto bonds = _connect.bonds();
    for (auto& bond : bonds){
        if (bond[0] == idx || bond[1] == idx)
            _connect.remove_bond(bond[0], bond[1]);
    }
    recalculate();
}

vector<bond> Topology::bonds() const{
    vector<bond> res;
    res.insert(begin(res), begin(_connect.bonds()), end(_connect.bonds()));
    return res;
}

vector<angle> Topology::angles() const{
    vector<angle> res;
    res.insert(begin(res), begin(_connect.angles()), end(_connect.angles()));
    return res;
}

vector<dihedral> Topology::dihedrals() const{
    vector<dihedral> res;
    res.insert(begin(res), begin(_connect.dihedrals()), end(_connect.dihedrals()));
    return res;
}

bool Topology::isbond(size_t i, size_t j) const  {
    auto bonds = _connect.bonds();
    auto pos = bonds.find(bond(i, j));
    return pos != end(bonds);
}

bool Topology::isangle(size_t i, size_t j, size_t k) const {
    auto angles = _connect.angles();
    auto pos = angles.find(angle(i, j, k));
    return pos != end(angles);
}

bool Topology::isdihedral(size_t i, size_t j, size_t k, size_t m) const {
    auto dihedrals = _connect.dihedrals();
    auto pos = dihedrals.find(dihedral(i, j, k, m));
    return pos != end(dihedrals);
}

void Topology::clear(){
    _templates.clear();
    _atoms.clear();
    _connect.clear();
}

Topology chemfiles::dummy_topology(size_t natoms){
    Topology top(0);
    for (size_t i=0; i<natoms; i++)
        top.append(Atom(Atom::UNDEFINED));
    return top;
}
