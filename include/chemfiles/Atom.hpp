/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_ATOM_HPP
#define CHEMFILES_ATOM_HPP

#include <string>

#include "chemfiles/exports.hpp"

namespace chemfiles {

/*!
 * @class Atom Atom.hpp Atom.cpp
 *
 * An Atom is a particle in the current Frame. It can be used to store and
 * retrieve informations about a particle, such as mass, name, atomic number,
 * etc.
 */
class CHFL_EXPORT Atom {
public:
    //! An Atom can be of various kind. This is encoded by a value in this enum.
    enum AtomType {
        //! Element from the periodic table of elements
        ELEMENT = 0,
        //! Coarse-grained atom are composed of more than one element: CH3
        //! groups, amino-acids are coarse-grained atoms.
        COARSE_GRAINED = 1,
        //! Dummy site, with no physical reality
        DUMMY = 2,
        //! Undefined atom-type
        UNDEFINED = 3,
    };

    //! Create an element from its `name`
    Atom(const std::string& name);
    //! Create an element from its `name` and its type
    Atom(AtomType type, const std::string& name = "");
    //! Default is to create an UNDEFINED atom type with no name
    Atom();

    Atom(const Atom&) = default;
    Atom& operator=(const Atom&) = default;

    ~Atom() = default;

    //! Get a const (non-modifiable) reference to the atom name
    const std::string& name() const { return name_; }
    //! Get the atom mass
    float mass() const { return mass_; }
    //! Get the atom charge
    float charge() const { return charge_; }
    //! Get the atom type
    AtomType type() const { return type_; }

    //! Set the atom name
    void set_name(const std::string& n) { name_ = n; }
    //! Set the atom mass
    void set_mass(float m) { mass_ = m; }
    //! Set the atom charge
    void set_charge(float c) { charge_ = c; }
    //! Set the atom type
    void set_type(AtomType t) { type_ = t; }

    //! Try to get the full element name, return and empty string if this is
    //! impossible
    std::string full_name() const;
    //! Try to get the Van der Waals of the atom. Returns -1 if it can not be
    //! found.
    float vdw_radius() const;
    //! Try to get the covalent radius of the atom. Returns -1 if it can not be
    //! found.
    float covalent_radius() const;
    //! Try to get the atomic number, if defined. Returns -1 if it can not be
    //! found.
    int atomic_number() const;

private:
    std::string name_;
    float mass_;
    float charge_;
    AtomType type_;
};

inline bool operator==(const Atom& lhs, const Atom& rhs) {
    return (lhs.name() == rhs.name() && lhs.mass() == rhs.mass() &&
            lhs.charge() == rhs.charge() && lhs.type() == rhs.type());
}

} // namespace chemfiles

#endif
