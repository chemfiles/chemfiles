/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Atom.hpp"
#include "periodic.hpp"

using namespace harp;

static bool str_in_vector(const string& s, const vector<string>& v);

Atom::Atom(const string& name) : _name(name), _mass(0), _charge(0) {
    if (str_in_vector(name, ALL_ELEMENTS))
        _type = ELEMENT;
    else
        _type = CORSE_GRAIN;

    if (PERIODIC_INFORMATION.find(name) != PERIODIC_INFORMATION.end()){
        _mass = PERIODIC_INFORMATION.at(name).mass ;
    }
}

Atom::Atom(AtomType type, const string& name) :
                                _name(name), _mass(0), _charge(0), _type(type){}

Atom::Atom() : Atom(UNDEFINED) {}

// Check if the string \c s is in the vector of strings \c v
static bool str_in_vector(const string& s, const vector<string>& v){
    for (size_t i=0; i<v.size(); i++){
        if (s == v[i]) {
            return true;
        }
    }
    return false;
}

std::string Atom::full_name() const {
    if (PERIODIC_INFORMATION.find(_name) != PERIODIC_INFORMATION.end()){
        return string(PERIODIC_INFORMATION.at(_name).name) ;
    }
    return "";
}

float Atom::vdw_radius() const {
    if (PERIODIC_INFORMATION.find(_name) != PERIODIC_INFORMATION.end()){
        return PERIODIC_INFORMATION.at(_name).vdw_radius ;
    }
    return -1;
}

float Atom::covalent_radius() const {
    if (PERIODIC_INFORMATION.find(_name) != PERIODIC_INFORMATION.end()){
        return PERIODIC_INFORMATION.at(_name).colvalent_radius ;
    }
    return -1;
}

int Atom::atomic_number() const {
    if (PERIODIC_INFORMATION.find(_name) != PERIODIC_INFORMATION.end()){
        return PERIODIC_INFORMATION.at(_name).number ;
    }
    return -1;
}
