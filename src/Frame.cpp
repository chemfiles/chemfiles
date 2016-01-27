/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <algorithm>

#include "chemfiles/Frame.hpp"
#include "chemfiles/Logger.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

Frame::Frame() : Frame(0) {}
Frame::Frame(size_t natoms): Frame(dummy_topology(natoms)) {}

Frame::Frame(const Topology& topology, const UnitCell& cell): _step(0), _topology(topology), _cell(cell) {
    resize(_topology.natoms());
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
    _positions.resize(size, vector3d(0.0, 0.0, 0.0));
    if (_velocities) {
        _velocities->resize(size, vector3d(0.0, 0.0, 0.0));
    }
}

void Frame::guess_topology(bool please_guess_bonds) {
    if (please_guess_bonds) {
        guess_bonds();
    }
    _topology.recalculate();
}

void Frame::guess_bonds() {
    _topology.clear_bonds();
    // This bond guessing algorithm comes from VMD
    double cutoff = 0.833;
    for (size_t i=0; i<natoms(); i++) {
        auto rad = _topology[i].vdw_radius();
        cutoff = fmax(cutoff, rad);
    }
    cutoff = 1.2 * cutoff;

    for (size_t i=0; i<natoms(); i++) {
        float irad = _topology[i].vdw_radius();
        if (irad == -1) {
            throw Error("Missing Van der Waals radius for the atom " + _topology[i].name());
        }
        for (size_t j=i+1; j<natoms(); j++) {
            float jrad = _topology[j].vdw_radius();
            if (jrad == -1) {
                throw Error("Missing Van der Waals radius for the atom " + _topology[j].name());
            }
            double d = norm(_cell.wrap(_positions[i] - _positions[j]));
            if (0.03 < d && d < 0.6 * (irad + jrad) && d < cutoff) {
                _topology.add_bond(i, j);
            }
        }
    }

    auto bonds = topology().bonds();
    auto to_remove = std::vector<Bond>();
    // We need to remove bonds between hydrogen atoms which are bonded more than once
    for (auto& bond: bonds) {
        auto i = bond[0], j = bond[1];
        if (_topology[i].name() != "H") {continue;}
        if (_topology[j].name() != "H") {continue;}

        auto nbonds = std::count_if(std::begin(bonds), std::end(bonds), [=](const Bond& b){
            return b[0] == i || b[0] == j || b[1] == i || b[1] == j;
        });
        assert(nbonds >= 1);

        if (nbonds != 1) {
            to_remove.push_back(bond);
        }
    }

    for (auto& bond: to_remove) {
        _topology.remove_bond(bond[0], bond[1]);
    }
}
