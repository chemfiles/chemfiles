// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "chemfiles/Atom.hpp"
#include "chemfiles/periodic.hpp"

using namespace chemfiles;

Atom::Atom(std::string name): Atom(name, name) {}

Atom::Atom(std::string name, std::string type):
    name_(std::move(name)), type_(std::move(type)) {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        mass_ = periodic->second.mass;
    }
}

std::string Atom::full_name() const {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second.name;
    } else {
        return "";
    }
}

double Atom::vdw_radius() const {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second.vdw_radius;
    } else {
        return -1;
    }
}

double Atom::covalent_radius() const {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second.colvalent_radius;
    } else {
        return -1;
    }
}

int Atom::atomic_number() const {
    auto periodic = PERIODIC_INFORMATION.find(type_);
    if (periodic != PERIODIC_INFORMATION.end()) {
        return periodic->second.number;
    } else {
        return -1;
    }
}
