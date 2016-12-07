// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <cstring>

#include "chemfiles/capi/topology.h"
#include "chemfiles/capi.hpp"

#include "chemfiles/Topology.hpp"
#include "chemfiles/Frame.hpp"
using namespace chemfiles;

extern "C" CHFL_TOPOLOGY* chfl_topology_from_frame(const CHFL_FRAME* const frame) {
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

extern "C" CHFL_TOPOLOGY* chfl_topology(void) {
    CHFL_TOPOLOGY* topology = nullptr;
    CHFL_ERROR_GOTO(
        topology = new Topology();
    )
    return topology;
error:
    delete topology;
    return nullptr;
}

extern "C" chfl_status chfl_topology_atoms_count(const CHFL_TOPOLOGY* const topology, uint64_t *natoms) {
    assert(topology != nullptr);
    assert(natoms != nullptr);
    CHFL_ERROR_CATCH(
        *natoms = topology->natoms();
    )
}

extern "C" chfl_status chfl_topology_resize(CHFL_TOPOLOGY* const topology, uint64_t natoms) {
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        topology->resize(checked_cast(natoms));
    )
}

extern "C" chfl_status chfl_topology_append(CHFL_TOPOLOGY* const topology, const CHFL_ATOM* const atom) {
    assert(topology != nullptr);
    assert(atom != nullptr);
    CHFL_ERROR_CATCH(
        topology->append(*atom);
    )
}

extern "C" chfl_status chfl_topology_remove(CHFL_TOPOLOGY* const topology, uint64_t i) {
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        topology->remove(checked_cast(i));
    )
}

extern "C" chfl_status chfl_topology_isbond(const CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_CATCH(
        *result = topology->isbond(checked_cast(i), checked_cast(j));
    )
}

extern "C" chfl_status chfl_topology_isangle(const CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j, uint64_t k, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_CATCH(
        *result = topology->isangle(checked_cast(i), checked_cast(j), checked_cast(k));
    )
}

extern "C" chfl_status chfl_topology_isdihedral(const CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j, uint64_t k, uint64_t m, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_CATCH(
        *result = topology->isdihedral(checked_cast(i), checked_cast(j), checked_cast(k), checked_cast(m));
    )
}

extern "C" chfl_status chfl_topology_bonds_count(const CHFL_TOPOLOGY* const topology, uint64_t* nbonds) {
    assert(topology != nullptr);
    assert(nbonds != nullptr);
    CHFL_ERROR_CATCH(
        *nbonds = topology->bonds().size();
    )
}

extern "C" chfl_status chfl_topology_angles_count(const CHFL_TOPOLOGY* const topology, uint64_t* nangles) {
    assert(topology != nullptr);
    assert(nangles != nullptr);
    CHFL_ERROR_CATCH(
        *nangles = topology->angles().size();
    )
}

extern "C" chfl_status chfl_topology_dihedrals_count(const CHFL_TOPOLOGY* const topology, uint64_t* ndihedrals) {
    assert(topology != nullptr);
    assert(ndihedrals != nullptr);
    CHFL_ERROR_CATCH(
        *ndihedrals = topology->dihedrals().size();
    )
}

#pragma intel optimization_level 2  /* Using -O3 with icc lead to partial copy of the bonds */
extern "C" chfl_status chfl_topology_bonds(const CHFL_TOPOLOGY* const topology, uint64_t (*data)[2], uint64_t nbonds) {
    assert(topology != nullptr);
    assert(data != nullptr);

    CHFL_ERROR_CATCH(
        if (nbonds != topology->bonds().size()) {
            CAPI_LAST_ERROR = "Wrong data size in function 'chfl_topology_bonds'.";
            return CHFL_MEMORY_ERROR;
        }

        auto& bonds = topology->bonds();
        for (size_t i=0; i<nbonds; i++) {
            data[i][0] = static_cast<uint64_t>(bonds[i][0]);
            data[i][1] = static_cast<uint64_t>(bonds[i][1]);
        }
    )
}

#pragma intel optimization_level 2 /* Using -O3 with icc lead to partial copy of the angles */
extern "C" chfl_status chfl_topology_angles(const CHFL_TOPOLOGY* const topology, uint64_t (*data)[3], uint64_t nangles) {
    assert(topology != nullptr);
    assert(data != nullptr);

    CHFL_ERROR_CATCH(
        if (nangles != topology->angles().size()) {
            CAPI_LAST_ERROR = "Wrong data size in function 'chfl_topology_angles'.";
            return CHFL_MEMORY_ERROR;
        }

        auto& angles = topology->angles();
        for (size_t i=0; i<nangles; i++) {
            data[i][0] = static_cast<uint64_t>(angles[i][0]);
            data[i][1] = static_cast<uint64_t>(angles[i][1]);
            data[i][2] = static_cast<uint64_t>(angles[i][2]);
        }
    )
}

#pragma intel optimization_level 2 /* Using -O3 with icc lead to partial copy of the dihedrals */
extern "C" chfl_status chfl_topology_dihedrals(const CHFL_TOPOLOGY* const topology, uint64_t (*data)[4], uint64_t ndihedrals) {
    assert(topology != nullptr);
    assert(data != nullptr);

    CHFL_ERROR_CATCH(
        if (ndihedrals != topology->dihedrals().size()) {
            CAPI_LAST_ERROR = "Wrong data size in function 'chfl_topology_bonds'.";
            return CHFL_MEMORY_ERROR;
        }

        auto& dihedrals = topology->dihedrals();
        for (size_t i=0; i<ndihedrals; i++) {
            data[i][0] = static_cast<uint64_t>(dihedrals[i][0]);
            data[i][1] = static_cast<uint64_t>(dihedrals[i][1]);
            data[i][2] = static_cast<uint64_t>(dihedrals[i][2]);
            data[i][3] = static_cast<uint64_t>(dihedrals[i][3]);
        }
    )
}

extern "C" chfl_status chfl_topology_add_bond(CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j) {
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        topology->add_bond(checked_cast(i), checked_cast(j));
    )
}

extern "C" chfl_status chfl_topology_remove_bond(CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j) {
    assert(topology != nullptr);
    CHFL_ERROR_CATCH(
        topology->remove_bond(checked_cast(i), checked_cast(j));
    )
}

extern "C" chfl_status chfl_topology_residues_count(const CHFL_TOPOLOGY* const topology, uint64_t* residues) {
    assert(topology != nullptr);
    assert(residues != nullptr);
    CHFL_ERROR_CATCH(
        *residues = topology->residues().size();
    )
}

extern "C" chfl_status chfl_topology_add_residue(CHFL_TOPOLOGY* const topology, const CHFL_RESIDUE* const residue) {
    assert(topology != nullptr);
    assert(residue != nullptr);
    CHFL_ERROR_CATCH(
        topology->add_residue(*residue);
    )
}

extern "C" chfl_status chfl_topology_are_linked(
    CHFL_TOPOLOGY* const topology, const CHFL_RESIDUE* const res_1,
    const CHFL_RESIDUE* const res_2, bool* result) {
    assert(topology != nullptr);
    assert(res_1 != nullptr);
    assert(res_2 != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_CATCH(
        *result = topology->are_linked(*res_1, *res_2);
    )
}


extern "C" chfl_status chfl_topology_free(CHFL_TOPOLOGY* const topology) {
    delete topology;
    return CHFL_SUCCESS;
}
