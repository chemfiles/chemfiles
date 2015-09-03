/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_ATOM_HPP
#define HARP_ATOM_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "chemharp/exports.hpp"

namespace harp {

/*!
 * @class Atom Atom.hpp Atom.cpp
 *
 * An Atom is a particle in the current Frame. It can be used to store and retrieve
 * informations about a particle, such as mass, name, atomic number, etc.
 */
class CHRP_EXPORT Atom {
public:
    //! An Atom can be of various kind. This is encoded by a value in this enum.
    enum AtomType {
        //! Element from the periodic table of elements
        ELEMENT,
        //! Corse-grained atom are composed of more than one element: CH3 groups,
        //! amino-acids are corse-grained atoms.
        CORSE_GRAIN,
        //! Dummy site, with no physical reality
        DUMMY,
        //! Undefined atom-type
        UNDEFINED
    };

    //! Create an element from its \c name
    Atom(const std::string& name);
    //! Create an element from its \c name and its type
    Atom(AtomType type, const std::string& name = "");
    //! Default is to create an UNDEFINED atom type with no name
    Atom();

    Atom(const Atom &) = default;
    Atom& operator=(const Atom &) = default;

    ~Atom() = default;

    //! Get a const (non-modifiable) reference to the atom name
    const std::string& name() const {return _name;}
    //! Get a const (non-modifiable) reference to the atom mass
    const float& mass() const {return _mass;}
    //! Get a const (non-modifiable) reference to the atom charge
    const float& charge() const {return _charge;}
    //! Get a const (non-modifiable) reference to the atom type
    const AtomType& type() const {return _type;}

    //! Set the atom name
    void name(const std::string& n) {_name = n;}
    //! Set the atom mass
    void mass(float m) {_mass = m;}
    //! Set the atom charge
    void charge(float c) {_charge = c;}
    //! Set the atom type
    void type(AtomType t) {_type = t;}

    //! Try to get the full element name, return and empty string if this is impossible
    std::string full_name() const;
    //! Try to get the Van der Waals of the atom. Returns -1 if it can not be found.
    float vdw_radius() const;
    //! Try to get the covalent radius of the atom. Returns -1 if it can not be found.
    float covalent_radius() const;
    //! Try to get the atomic number, if defined. Returns -1 if it can not be found.
    int atomic_number() const;
private:
    std::string _name;
    float _mass;
    float _charge;
    AtomType _type;
};

inline bool operator==(const Atom& lhs, const Atom& rhs) {
    return (lhs.name() == rhs.name() && lhs.mass() == rhs.mass() &&
            lhs.charge() == rhs.charge() && lhs.type() == rhs.type());
}

inline std::ostream& operator<<(std::ostream& out, const Atom& atom){
    out << "Atom \"" << atom.name() << "\"";
    return out;
}

//! All the elements in the periodic table
const std::vector<std::string> ALL_ELEMENTS = std::vector<std::string>{
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

} // namespace harp

#endif
