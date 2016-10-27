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

CHFL_ATOM* chfl_atom(const char* name) {
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_GOTO(
        atom = new Atom(name);
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

CHFL_ATOM* chfl_atom_from_frame(const CHFL_FRAME* const frame, uint64_t idx) {
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

CHFL_ATOM* chfl_atom_from_topology(const CHFL_TOPOLOGY* const topology, uint64_t idx) {
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

chfl_status chfl_atom_mass(const CHFL_ATOM* const atom, double* mass) {
    assert(atom != nullptr);
    assert(mass != nullptr);
    CHFL_ERROR_CATCH(
        *mass = atom->mass();
    )
}

chfl_status chfl_atom_set_mass(CHFL_ATOM* const atom, double mass) {
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_mass(mass);
    )
}

chfl_status chfl_atom_charge(const CHFL_ATOM* const atom, double* charge) {
    assert(atom != nullptr);
    assert(charge != nullptr);
    CHFL_ERROR_CATCH(
        *charge = atom->charge();
    )
}

chfl_status chfl_atom_set_charge(CHFL_ATOM* const atom, double charge) {
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_charge(charge);
    )
}

chfl_status chfl_atom_type(const CHFL_ATOM* const atom, char* const type, uint64_t buffsize) {
    assert(atom != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(type, atom->type().c_str(), buffsize - 1);
        type[buffsize - 1] = '\0';
    )
}

chfl_status chfl_atom_set_type(CHFL_ATOM* const atom, const char* type) {
    assert(atom != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_type(type);
    )
}

chfl_status chfl_atom_name(const CHFL_ATOM* const atom, char* const name, uint64_t buffsize) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(name, atom->name().c_str(), buffsize - 1);
        name[buffsize - 1] = '\0';
    )
}

chfl_status chfl_atom_set_name(CHFL_ATOM* const atom, const char* name) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_name(name);
    )
}

chfl_status chfl_atom_full_name(const CHFL_ATOM* const atom, char* const name, uint64_t buffsize) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(name, atom->full_name().c_str(), buffsize - 1);
        name[buffsize - 1] = '\0';
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

chfl_status chfl_atom_atomic_number(const CHFL_ATOM* const atom, int64_t* number) {
    assert(atom != nullptr);
    assert(number != nullptr);
    CHFL_ERROR_CATCH(
        *number = atom->atomic_number();
    )
}

chfl_status chfl_atom_free(CHFL_ATOM* atom) {
    delete atom;
    atom = nullptr;
    return CHFL_SUCCESS;
}
