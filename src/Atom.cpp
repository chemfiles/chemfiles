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

// Check if the string `element` is an element
static bool is_element(const std::string& element) {
    // clang-format off
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
    // clang-format on
    for (auto& each_element : ALL_ELEMENTS) {
        if (element == each_element) {
            return true;
        }
    }
    return false;

}

Atom::Atom(std::string element)
	: element_(std::move(element)), label_(std::move(element)), mass_(0),
	charge_(0) {
    if (is_element(element_)) {
        type_ = ELEMENT;
    } else {
        type_ = COARSE_GRAINED;
    }

    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        mass_ = PERIODIC_INFORMATION.at(element_).mass;
    }
}

Atom::Atom(std::string element, std::string label)
	: element_(std::move(element)), label_(std::move(label)), mass_(0),
	charge_(0) {
    if (is_element(element_)) {
        type_ = ELEMENT;
    } else {
        type_ = COARSE_GRAINED;
    }

    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        mass_ = PERIODIC_INFORMATION.at(element_).mass;
    }
}

Atom::Atom(AtomType type, std::string element, std::string label)
    : element_(std::move(element)), label_(std::move(label)), mass_(0),
	charge_(0), type_(type) {}

Atom::Atom() : Atom(UNDEFINED) {}

std::string Atom::full_name() const {
    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        return std::string(PERIODIC_INFORMATION.at(element_).name);
    }
    return "";
}

float Atom::vdw_radius() const {
    if (PERIODIC_INFORMATION.find(element_) != PERIODIC_INFORMATION.end()) {
        return PERIODIC_INFORMATION.at(element_).vdw_radius;
    }
    return -1;
}

float Atom::covalent_radius() const {
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
