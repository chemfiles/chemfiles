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

void Frame::raw_positions(float pos[][3], size_t size) const{
    if (size < _positions.size())
        throw MemoryError("Too small array passed to get_raw_positions.");
    for (size_t i = 0; i<size; i++) {
        for (size_t j = 0; j<3; j++) {
            pos[i][j] = _positions[i][j];
        }
    }
}

void Frame::raw_velocities(float vel[][3], size_t size) const{
    if (size < _velocities.size())
        throw MemoryError("Too small array passed to get_raw_velocities.");
    if (_velocities.empty()){ // Filling the matrix with zeroes
        for (size_t i = 0; i<size; i++)
            for (size_t j = 0; j<3; j++)
                vel[i][j] = 0;
    }
    else {
        for (size_t i = 0; i<size; i++)
            for (size_t j = 0; j<3; j++)
                vel[i][j] = _velocities[i][j];
    }
}

size_t Frame::natoms() const {
    auto npos = _positions.size();
    auto nvel = _velocities.size();

    if (npos == nvel || nvel == 0 /* No velocity data */) {
        return npos;
    }
    else {
        LOG(DEBUG) << "Inconsistent size in frame." << std::endl;
        return npos;
    }
}

void Frame::resize(size_t size, bool reserve_velocities){
    _positions.resize(size, 0.0);
    if (reserve_velocities) {
        _velocities.resize(size, 0.0);
    }
}

bool Frame::has_velocities() const{
    return _velocities.size() == _positions.size() && _velocities.size() > 0;
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
