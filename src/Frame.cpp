/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <iostream>

#include "chemfiles/Frame.hpp"
#include "chemfiles/Logger.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

Frame::Frame() : Frame(0) {}

Frame::Frame(size_t natoms) : _step(0), _topology(natoms), _cell() {
    resize(natoms);
}

size_t Frame::natoms() const {
    if (!_velocities) {
        return _positions.size();
    }

    auto npos = _positions.size();
    auto nvel = _velocities->size();

    if (npos != nvel) {
        LOG(WARNING) << "Inconsistent size in frame. Positions contains " << npos << " atoms, but velocities contains " << nvel << " atoms." << std::endl;
    }

    return npos;
}

void Frame::resize(size_t size){
    _positions.resize(size, 0.0);
    if (_velocities) {
        _velocities->resize(size, 0.0);
    }
}

void Frame::guess_topology(bool please_guess_bonds) {
    if (please_guess_bonds) {
        guess_bonds();
    }
    _topology.recalculate();
}

void Frame::guess_bonds() {
    for (size_t i=0; i<natoms(); i++) {
        float irad = _topology[i].covalent_radius();
        if (irad == -1) {
            throw Error("Missing covalent radius for the atom " + _topology[i].name());
        }
        for (size_t j=0; j<natoms(); j++) {
            float jrad = _topology[j].covalent_radius();
            if (jrad == -1) {
                throw Error("Missing covalent radius for the atom " + _topology[j].name());
            }
            double d = norm(_cell.wrap(_positions[i] - _positions[j]));
            if (d > 0.4 && d < irad + jrad + 0.56) { // This criterium comes from Rasmol
                _topology.add_bond(i, j);
            }
        }
    }
}
