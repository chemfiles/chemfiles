/*
 * Harp, an efficient IO library for chemistry file formats
 * Copyright (C) 2014 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_ATOM_HPP
#define HARP_ATOM_HPP

#include <string>
#include <vector>
#include <map>

namespace harp {

using std::string;
using std::vector;
using std::map;

/*!
 * @class Atom Atom.hpp Atom.cpp
 * @brief Atom representation class
 */
class Atom {
public:
    enum AtomType {
        //! Element from the periodic table of elements
        ELEMENT,
        //! Corse-grained atom: CH4, …
        CORSE_GRAIN,
        //! Dummy site, with no physical reality
        DUMMY,
        //! Undefined atom-type
        UNDEFINED
    };

    Atom(const std::string& name);
    Atom(AtomType type, const string& name = "");
    Atom();
    Atom(const Atom &) = default;
    Atom& operator=(const Atom &) = default;

    ~Atom(){};

    //! Get a const (non-modifiable) reference to the atom name
    const string& name(void) const {return _name;}
    //! Get a const (non-modifiable) reference to the atom mass
    const float& mass(void) const {return _mass;}
    //! Get a const (non-modifiable) reference to the atom charge
    const float& charge(void) const {return _charge;}
    //! Get a const (non-modifiable) reference to the atom type
    const AtomType& type(void) const {return _type;}

    //! Set the atom name
    void name(string n) {_name = n;}
    //! Set the atom mass
    void mass(float m) {_mass = m;}
    //! Set the atom charge
    void charge(float c) {_charge = c;}
    //! Set the atom type
    void type(AtomType t) {_type = t;}

private:
    string _name;
    float _mass;
    float _charge;
    AtomType _type;
};

inline bool operator==(const Atom& a, const Atom& b){
    return (a.name() == b.name() && a.mass() == b.mass() &&
            a.charge() == b.charge() && a.type() == b.type());
}

//! All the elements in the periodic table
const vector<string> ALL_ELEMENTS = vector<string>{
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

//! Name->mass mapping
const map<string, float> ELEMENTS_MASSES = map<string, float>{
    {"H", 1.01} //TODO: other values
};

} // namespace harp

#endif
