/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Atom.hpp"

using namespace harp;

bool str_in_vector(const string& s, const vector<string>& v);

Atom::Atom(const string& name) : _name(name), _mass(0), _charge(0) {
    if (str_in_vector(name, ALL_ELEMENTS))
        _type = ELEMENT;
    else
        _type = CORSE_GRAIN;

    // TODO: get mass
}

Atom::Atom(AtomType type, const string& name) :
                                _name(name), _mass(0), _charge(0), _type(type){}

Atom::Atom() : Atom(UNDEFINED) {}

// Check if the string \c s is in the vector of strings \c v
inline bool str_in_vector(const string& s, const vector<string>& v){
    for (int i=0; i<v.size(); i++){
        if (s == v[i]) {
            return true;
        }
    }
    return false;
}
