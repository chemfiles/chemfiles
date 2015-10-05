/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp/Atom.hpp"
#include "chemharp/periodic.hpp"

using namespace harp;

// Check if the string \c name is the name of an element
static bool is_element(const std::string& name){
    const auto ALL_ELEMENTS = {
    "H" ,                                                                                                 "He",
    "Li", "Be",                                                             "B" , "C" , "N" , "O" , "F" , "Ne",
    "Na", "Mg",                                                             "Al", "Si", "P" , "S" , "Cl", "Ar",
    "K" , "Ca", "Sc", "Ti", "V" , "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr",
    "Rb", "Sr", "Y" , "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ar", "Cd", "In", "Sn", "Sb", "Te", "I" , "Xe",
    "Cs", "Ba", "La", "Hf", "Ta", "W" , "Re", "Os", "Ir", "Pt", "Au", "Hg", "Ti", "Pb", "Bi", "Po", "At", "Rn",
    "Fr", "Ra", "Ac", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn",

    "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu",
    "Th", "Pa", "U" , "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr"
    };
    for (auto& element: ALL_ELEMENTS){
        if (name == element) {
            return true;
        }
    }
    return false;
}

Atom::Atom(const std::string& name) : _name(name), _mass(0), _charge(0) {
    if (is_element(name)) {
        _type = ELEMENT;
    } else {
        _type = CORSE_GRAIN;
    }

    if (PERIODIC_INFORMATION.find(name) != PERIODIC_INFORMATION.end()){
        _mass = PERIODIC_INFORMATION.at(name).mass ;
    }
}

Atom::Atom(AtomType type, const std::string& name) : _name(name), _mass(0), _charge(0), _type(type){}

Atom::Atom() : Atom(UNDEFINED) {}

std::string Atom::full_name() const {
    if (PERIODIC_INFORMATION.find(_name) != PERIODIC_INFORMATION.end()){
        return std::string(PERIODIC_INFORMATION.at(_name).name) ;
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
