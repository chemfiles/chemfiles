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
 * retrieve informations about a particle, such as mass, element name, atomic
 * number, etc.
 */
class CHFL_EXPORT Atom {
public:
    //! Create an atom with the given `label` and set `element` to be the same
    //! as label.
    explicit Atom(std::string label = "");
    //! Create an atom from the given `label` and `element`
    Atom(std::string element, std::string label);

    Atom(Atom&&) = default;
    Atom& operator=(Atom&&) = default;
    Atom(const Atom&) = default;
    Atom& operator=(const Atom&) = default;

    //! Get a const (non-modifiable) reference to the atom element name
    const std::string& element() const { return element_; }
    //! Get a const (non-modifiable) reference to the atom label
    const std::string& label() const { return label_; }
    //! Get the atom mass
    double mass() const { return mass_; }
    //! Get the atom charge
    double charge() const { return charge_; }

    //! Set the atom element name
    void set_element(const std::string& element) { element_ = element; }
    //! Set the atom label
    void set_label(const std::string& label) { label_ = label; }
    //! Set the atom mass
    void set_mass(double mass) { mass_ = mass; }
    //! Set the atom charge
    void set_charge(double charge) { charge_ = charge; }

    //! Try to get the full element name, return and empty string if this is
    //! impossible
    std::string full_name() const;
    //! Try to get the Van der Waals of the atom. Returns -1 if it can not be
    //! found.
    double vdw_radius() const;
    //! Try to get the covalent radius of the atom. Returns -1 if it can not be
    //! found.
    double covalent_radius() const;
    //! Try to get the atomic number, if defined. Returns -1 if it can not be
    //! found.
    int atomic_number() const;

private:
    std::string label_;
    std::string element_;
    double mass_ = 0;
    double charge_ = 0;
};

inline bool operator==(const Atom& lhs, const Atom& rhs) {
    return (lhs.element() == rhs.element() && lhs.label() == rhs.label() &&
            lhs.mass() == rhs.mass() && lhs.charge() == rhs.charge());
}

} // namespace chemfiles

#endif
