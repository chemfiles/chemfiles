/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
// clang-format off

#include <cstring>

#include "chemfiles.h"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

CHFL_ATOM* chfl_atom(const char* element) {
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_GOTO(
        atom = new Atom(std::string(element));
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

CHFL_ATOM* chfl_atom_from_frame(const CHFL_FRAME* const frame, size_t idx) {
    assert(frame != nullptr);
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_GOTO(
        // Return NULL if the index is out of bounds
        if (idx < frame->natoms()) {
            atom = new Atom("");
            *atom = frame->topology()[idx];
        }
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

CHFL_ATOM* chfl_atom_from_topology(const CHFL_TOPOLOGY* const topology, size_t idx) {
    assert(topology != nullptr);
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_GOTO(
        // Return NULL if the index is out of bounds
        if (idx < topology->natoms()) {
            atom = new Atom("");
            *atom = (*topology)[idx];
        }
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

chfl_status chfl_atom_mass(const CHFL_ATOM* const atom, float* mass) {
    assert(atom != nullptr);
    assert(mass != nullptr);
    CHFL_ERROR_CATCH(
        *mass = atom->mass();
    )
}

chfl_status chfl_atom_set_mass(CHFL_ATOM* const atom, float mass) {
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_mass(mass);
    )
}

chfl_status chfl_atom_charge(const CHFL_ATOM* const atom, float* charge) {
    assert(atom != nullptr);
    assert(charge != nullptr);
    CHFL_ERROR_CATCH(
        *charge = atom->charge();
    )
}

chfl_status chfl_atom_set_charge(CHFL_ATOM* const atom, float charge) {
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_charge(charge);
    )
}

chfl_status chfl_atom_element(const CHFL_ATOM* const atom, char* const element, size_t buffsize) {
    assert(atom != nullptr);
    assert(element != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(element, atom->element().c_str(), buffsize - 1);
        element[buffsize - 1] = '\0';
    )
}

chfl_status chfl_atom_set_element(CHFL_ATOM* const atom, const char* element) {
    assert(atom != nullptr);
    assert(element != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_element(std::string(element));
    )
}

chfl_status chfl_atom_label(const CHFL_ATOM* const atom, char* const label, size_t buffsize) {
    assert(atom != nullptr);
    assert(label != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(label, atom->label().c_str(), buffsize - 1);
        label[buffsize - 1] = '\0';
    )
}

chfl_status chfl_atom_set_label(CHFL_ATOM* const atom, const char* label) {
    assert(atom != nullptr);
    assert(label != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_element(std::string(label));
    )
}

chfl_status chfl_atom_full_name(const CHFL_ATOM* const atom, char* const element, size_t buffsize) {
    assert(atom != nullptr);
    assert(element != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(element, atom->full_name().c_str(), buffsize - 1);
        element[buffsize - 1] = '\0';
    )
}

chfl_status chfl_atom_vdw_radius(const CHFL_ATOM* const atom, double* radius) {
    assert(atom != nullptr);
    assert(radius != nullptr);
    CHFL_ERROR_CATCH(
        *radius = atom->vdw_radius();
    )
}

chfl_status chfl_atom_covalent_radius(const CHFL_ATOM* const atom, double* radius) {
    assert(atom != nullptr);
    assert(radius != nullptr);
    CHFL_ERROR_CATCH(
        *radius = atom->covalent_radius();
    )
}

chfl_status chfl_atom_atomic_number(const CHFL_ATOM* const atom, int* number) {
    assert(atom != nullptr);
    assert(number != nullptr);
    CHFL_ERROR_CATCH(
        *number = atom->atomic_number();
    )
}

chfl_status chfl_atom_type(const CHFL_ATOM* const atom, chfl_atom_type_t* const type) {
    assert(atom != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_CATCH(
        *type = static_cast<chfl_atom_type_t>(atom->type());
    )
}

chfl_status chfl_atom_set_type(CHFL_ATOM* const atom, chfl_atom_type_t type) {
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_type(static_cast<Atom::AtomType>(type));
    )
}

chfl_status chfl_atom_free(CHFL_ATOM* atom) {
    delete atom;
    atom = nullptr;
    return CHFL_SUCCESS;
}
