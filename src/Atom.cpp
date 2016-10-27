/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemfiles/Atom.hpp"
#include "chemfiles/periodic.hpp"

using namespace chemfiles;

Atom::Atom(std::string label): Atom(label, label) {}

Atom::Atom(std::string element, std::string label):
    label_(std::move(label)), element_(std::move(element)) {
    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        mass_ = PERIODIC_INFORMATION.at(element_).mass;
    }
}

std::string Atom::full_name() const {
    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        return std::string(PERIODIC_INFORMATION.at(element_).name);
    }
    return "";
}

double Atom::vdw_radius() const {
    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        return PERIODIC_INFORMATION.at(element_).vdw_radius;
    }
    return -1;
}

double Atom::covalent_radius() const {
    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        return PERIODIC_INFORMATION.at(element_).colvalent_radius;
    }
    return -1;
}

int Atom::atomic_number() const {
    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        return PERIODIC_INFORMATION.at(element_).number;
    }
    return -1;
}
