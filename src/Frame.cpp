/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include <iostream>

#include "Frame.hpp"
#include "Logger.hpp"
#include "Error.hpp"

using namespace harp;

Frame::Frame() : Frame(100) {}

Frame::Frame(size_t natoms) : _step(0), _topology(natoms), _cell() {
    reserve(natoms);
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

void Frame::reserve(size_t size, bool reserve_velocities){
    _positions.reserve(size);
    _positions.assign(size, Vector3D(0, 0, 0));
    if (reserve_velocities) {
        _velocities.reserve(size);
        _velocities.assign(size, Vector3D(0, 0, 0));
    }
    _topology.reserve(size);
}

bool Frame::has_velocities(){
    return _velocities.size() == _positions.size() && _velocities.size() > 0;
}
