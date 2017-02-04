// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <algorithm>

#include "chemfiles/Error.hpp"
#include "chemfiles/Frame.hpp"
using namespace chemfiles;

Frame::Frame() : Frame(0) {}
Frame::Frame(size_t natoms) : Frame(Topology()) {
    resize(natoms);
}

Frame::Frame(Topology topology, UnitCell cell)
    : step_(0), topology_(std::move(topology)), cell_(std::move(cell)) {
    resize(topology_.natoms());
}

size_t Frame::natoms() const {
    assert(positions_.size() == topology_.natoms());
    if (velocities_) {
        assert(positions_.size() == velocities_->size());
    }
    return positions_.size();
}

void Frame::resize(size_t natoms) {
    topology_.resize(natoms);
    positions_.resize(natoms, vector3d(0.0, 0.0, 0.0));
    if (velocities_) {
        velocities_->resize(natoms, vector3d(0.0, 0.0, 0.0));
    }
}

void Frame::reserve(size_t natoms) {
    topology_.reserve(natoms);
    positions_.reserve(natoms);
    if (velocities_) {
        velocities_->reserve(natoms);
    }
}

void Frame::add_velocities() {
    if (!velocities_) {
        velocities_ = Array3D(natoms(), vector3d(0.0, 0.0, 0.0));
    }
}

void Frame::guess_topology() {
    topology_.clear_bonds();
    // This bond guessing algorithm comes from VMD
    auto cutoff = 0.833;
    for (size_t i = 0; i < natoms(); i++) {
        auto rad = topology_[i].vdw_radius();
        cutoff = fmax(cutoff, rad);
    }
    cutoff = 1.2 * cutoff;

    for (size_t i = 0; i < natoms(); i++) {
        auto irad = topology_[i].vdw_radius();
        if (irad <= 0) {
            throw Error(
                "Missing Van der Waals radius for '" + topology_[i].type() + "'"
            );
        }
        for (size_t j = i + 1; j < natoms(); j++) {
            auto jrad = topology_[j].vdw_radius();
            if (jrad <= 0) {
                throw Error(
                    "Missing Van der Waals radius for '" + topology_[j].type() + "'"
                );
            }
            auto d = norm(cell_.wrap(positions_[i] - positions_[j]));
            if (0.03 < d && d < 0.6 * (irad + jrad) && d < cutoff) {
                topology_.add_bond(i, j);
            }
        }
    }

    auto bonds = topology().bonds();
    auto to_remove = std::vector<Bond>();
    // We need to remove bonds between hydrogen atoms which are bonded more than
    // once
    for (auto& bond : bonds) {
        auto i = bond[0], j = bond[1];
        if (topology_[i].type() != "H") {
            continue;
        }
        if (topology_[j].type() != "H") {
            continue;
        }

        auto nbonds = std::count_if(
            std::begin(bonds), std::end(bonds), [=](const Bond& b) {
                return b[0] == i || b[0] == j || b[1] == i || b[1] == j;
            });
        assert(nbonds >= 1);

        if (nbonds != 1) {
            to_remove.push_back(bond);
        }
    }

    for (auto& bond : to_remove) {
        topology_.remove_bond(bond[0], bond[1]);
    }
}

void Frame::set_topology(const Topology& topology) {
    if (topology.natoms() != positions_.size()) {
        throw Error("the topology contains " +
                    std::to_string(topology.natoms()) +
                    " atoms, but the frame contains " +
                    std::to_string(positions_.size()) + " atoms.");
    }
    topology_ = topology;
}

void Frame::add_atom(Atom atom, Vector3D position, Vector3D velocity) {
    topology_.append(std::move(atom));
    positions_.push_back(position);
    if (velocities_) {
        velocities_->push_back(velocity);
    }
    assert(natoms() == topology_.natoms());
}

void Frame::remove(size_t i) {
    assert(i < natoms() && "Can not remove out of bounds atom");
    topology_.remove(i);
    positions_.erase(positions_.begin() + static_cast<std::ptrdiff_t>(i));
    if (velocities_) {
        velocities_->erase(velocities_->begin() + static_cast<std::ptrdiff_t>(i));
    }
    assert(natoms() == topology_.natoms());
}
