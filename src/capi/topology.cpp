/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
// clang-format off

#include "chemfiles.h"
#include "chemfiles/Topology.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

CHFL_TOPOLOGY* chfl_topology_from_frame(const CHFL_FRAME* frame) {
    assert(frame != nullptr);
    CHFL_TOPOLOGY* topology = nullptr;
    CHFL_ERROR_GOTO(
        topology = new Topology();
        *topology = frame->topology();
    )
    return topology;
error:
    delete topology;
    return nullptr;
}

CHFL_TOPOLOGY* chfl_topology(void) {
    CHFL_TOPOLOGY* topology = nullptr;
    CHFL_ERROR_GOTO(
        topology = new Topology();
    )
    return topology;
error:
    delete topology;
    return nullptr;
}

int chfl_topology_atoms_count(const CHFL_TOPOLOGY* topology, size_t *natoms) {
    assert(topology != nullptr);
    assert(natoms != nullptr);
    CHFL_ERROR_CATCH(
        *natoms = topology->natoms();
    )
}

int chfl_topology_append(CHFL_TOPOLOGY* topology, const CHFL_ATOM* atom) {
    assert(topology != nullptr);
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        topology->append(*atom);
    )
}

int chfl_topology_remove(CHFL_TOPOLOGY* topology, size_t i) {
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        topology->remove(i);
    )
}

int chfl_topology_isbond(const CHFL_TOPOLOGY* topology, size_t i, size_t j, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_CATCH(
        *result = topology->isbond(i, j);
    )
}

int chfl_topology_isangle(const CHFL_TOPOLOGY* topology, size_t i, size_t j, size_t k, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_CATCH(
        *result = topology->isangle(i, j, k);
    )
}

int chfl_topology_isdihedral(const CHFL_TOPOLOGY* topology, size_t i, size_t j, size_t k, size_t m, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_CATCH(
        *result = topology->isdihedral(i, j, k, m);
    )
}

int chfl_topology_bonds_count(const CHFL_TOPOLOGY* topology, size_t* nbonds) {
    assert(topology != nullptr);
    assert(nbonds != nullptr);
    CHFL_ERROR_CATCH(
        *nbonds = topology->bonds().size();
    )
}

int chfl_topology_angles_count(const CHFL_TOPOLOGY* topology, size_t* nangles) {
    assert(topology != nullptr);
    assert(nangles != nullptr);
    CHFL_ERROR_CATCH(
        *nangles = topology->angles().size();
    )
}

int chfl_topology_dihedrals_count(const CHFL_TOPOLOGY* topology, size_t* ndihedrals) {
    assert(topology != nullptr);
    assert(ndihedrals != nullptr);
    CHFL_ERROR_CATCH(
        *ndihedrals = topology->dihedrals().size();
    )
}

int chfl_topology_bonds(const CHFL_TOPOLOGY* topology, size_t (*data)[2], size_t nbonds) {
    assert(topology != nullptr);
    assert(data != nullptr);
    if (nbonds != topology->bonds().size()) {
        CAPI_LAST_ERROR = "Wrong data size in function 'chfl_topology_bonds'.";
        return CHFL_MEMORY_ERROR;
    }

    auto bonds = topology->bonds();
    CHFL_ERROR_CATCH(
        for (size_t i = 0; i<nbonds; i++) {
            data[i][0] = bonds[i][0];
            data[i][1] = bonds[i][1];
        }
    )
}

int chfl_topology_angles(const CHFL_TOPOLOGY* topology, size_t (*data)[3], size_t nangles) {
    assert(topology != nullptr);
    assert(data != nullptr);
    if (nangles != topology->angles().size()) {
        CAPI_LAST_ERROR = "Wrong data size in function 'chfl_topology_angles'.";
        return CHFL_MEMORY_ERROR;
    }

    auto angles = topology->angles();
    CHFL_ERROR_CATCH(
        for (size_t i = 0; i<nangles; i++) {
            data[i][0] = angles[i][0];
            data[i][1] = angles[i][1];
            data[i][2] = angles[i][2];
        }
    )
}

int chfl_topology_dihedrals(const CHFL_TOPOLOGY* topology, size_t (*data)[4], size_t ndihedrals) {
    assert(topology != nullptr);
    assert(data != nullptr);
    if (ndihedrals != topology->dihedrals().size()) {
        CAPI_LAST_ERROR = "Wrong data size in function 'chfl_topology_bonds'.";
        return CHFL_MEMORY_ERROR;
    }

    auto dihedrals = topology->dihedrals();
    CHFL_ERROR_CATCH(
        for (size_t i = 0; i<ndihedrals; i++) {
            data[i][0] = dihedrals[i][0];
            data[i][1] = dihedrals[i][1];
            data[i][2] = dihedrals[i][2];
            data[i][3] = dihedrals[i][3];
        }
    )
}

int chfl_topology_add_bond(CHFL_TOPOLOGY* topology, size_t i, size_t j) {
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        topology->add_bond(i, j);
    )
}

int chfl_topology_remove_bond(CHFL_TOPOLOGY* topology, size_t i, size_t j) {
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        topology->remove_bond(i, j);
    )
}

int chfl_topology_free(CHFL_TOPOLOGY* topology) {
    delete topology;
    topology = nullptr;
    return CHFL_SUCCESS;
}
