// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <cstring>

#include "chemfiles/capi/atom.h"
#include "chemfiles/capi.hpp"

#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Topology.hpp"
using namespace chemfiles;

extern "C" CHFL_ATOM* chfl_atom(const char* name) {
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_GOTO(
        atom = new Atom(name);
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

extern "C" CHFL_ATOM* chfl_atom_from_frame(const CHFL_FRAME* const frame, uint64_t idx) {
    assert(frame != nullptr);
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_GOTO(
        // Return NULL if the index is out of bounds
        if (idx < frame->natoms()) {
            atom = new Atom("");
            *atom = frame->topology()[checked_cast(idx)];
        }
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

extern "C" CHFL_ATOM* chfl_atom_from_topology(const CHFL_TOPOLOGY* const topology, uint64_t idx) {
    assert(topology != nullptr);
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_GOTO(
        // Return NULL if the index is out of bounds
        if (idx < topology->natoms()) {
            atom = new Atom("");
            *atom = (*topology)[checked_cast(idx)];
        }
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

extern "C" chfl_status chfl_atom_mass(const CHFL_ATOM* const atom, double* mass) {
    assert(atom != nullptr);
    assert(mass != nullptr);
    CHFL_ERROR_CATCH(
        *mass = atom->mass();
    )
}

extern "C" chfl_status chfl_atom_set_mass(CHFL_ATOM* const atom, double mass) {
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_mass(mass);
    )
}

extern "C" chfl_status chfl_atom_charge(const CHFL_ATOM* const atom, double* charge) {
    assert(atom != nullptr);
    assert(charge != nullptr);
    CHFL_ERROR_CATCH(
        *charge = atom->charge();
    )
}

extern "C" chfl_status chfl_atom_set_charge(CHFL_ATOM* const atom, double charge) {
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_charge(charge);
    )
}

extern "C" chfl_status chfl_atom_type(const CHFL_ATOM* const atom, char* const type, uint64_t buffsize) {
    assert(atom != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(type, atom->type().c_str(), checked_cast(buffsize) - 1);
        type[buffsize - 1] = '\0';
    )
}

extern "C" chfl_status chfl_atom_set_type(CHFL_ATOM* const atom, const char* type) {
    assert(atom != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_type(type);
    )
}

extern "C" chfl_status chfl_atom_name(const CHFL_ATOM* const atom, char* const name, uint64_t buffsize) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(name, atom->name().c_str(), checked_cast(buffsize) - 1);
        name[buffsize - 1] = '\0';
    )
}

extern "C" chfl_status chfl_atom_set_name(CHFL_ATOM* const atom, const char* name) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_CATCH(
        atom->set_name(name);
    )
}

extern "C" chfl_status chfl_atom_full_name(const CHFL_ATOM* const atom, char* const name, uint64_t buffsize) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_CATCH(
        strncpy(name, atom->full_name().c_str(), checked_cast(buffsize) - 1);
        name[buffsize - 1] = '\0';
    )
}

extern "C" chfl_status chfl_atom_vdw_radius(const CHFL_ATOM* const atom, double* radius) {
    assert(atom != nullptr);
    assert(radius != nullptr);
    CHFL_ERROR_CATCH(
        *radius = atom->vdw_radius();
    )
}

extern "C" chfl_status chfl_atom_covalent_radius(const CHFL_ATOM* const atom, double* radius) {
    assert(atom != nullptr);
    assert(radius != nullptr);
    CHFL_ERROR_CATCH(
        *radius = atom->covalent_radius();
    )
}

extern "C" chfl_status chfl_atom_atomic_number(const CHFL_ATOM* const atom, int64_t* number) {
    assert(atom != nullptr);
    assert(number != nullptr);
    CHFL_ERROR_CATCH(
        *number = atom->atomic_number();
    )
}

extern "C" chfl_status chfl_atom_free(CHFL_ATOM* const atom) {
    delete atom;
    return CHFL_SUCCESS;
}
