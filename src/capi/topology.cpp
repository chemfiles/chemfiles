// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstring>

#include "chemfiles/capi/topology.h"
#include "chemfiles/capi.hpp"

#include "chemfiles/Topology.hpp"
#include "chemfiles/Frame.hpp"
using namespace chemfiles;

extern "C" CHFL_TOPOLOGY* chfl_topology_from_frame(const CHFL_FRAME* const frame) {
    CHFL_TOPOLOGY* topology = nullptr;
    CHECK_POINTER_GOTO(frame);
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

extern "C" CHFL_TOPOLOGY* chfl_topology_copy(const CHFL_TOPOLOGY* const topology) {
    CHFL_TOPOLOGY* new_topology = nullptr;
    CHFL_ERROR_GOTO(
        new_topology = new Topology(*topology);
    )
    return new_topology;
error:
    delete new_topology;
    return nullptr;
}

extern "C" chfl_status chfl_topology_atoms_count(const CHFL_TOPOLOGY* const topology, uint64_t *size) {
    CHECK_POINTER(topology);
    CHECK_POINTER(size);
    CHFL_ERROR_CATCH(
        *size = topology->size();
    )
}

extern "C" chfl_status chfl_topology_resize(CHFL_TOPOLOGY* const topology, uint64_t natoms) {
    CHECK_POINTER(topology);
    CHFL_ERROR_CATCH(
        topology->resize(checked_cast(natoms));
    )
}

extern "C" chfl_status chfl_topology_add_atom(CHFL_TOPOLOGY* const topology, const CHFL_ATOM* const atom) {
    CHECK_POINTER(topology);
    CHECK_POINTER(atom);
    CHFL_ERROR_CATCH(
        topology->add_atom(*atom);
    )
}

extern "C" chfl_status chfl_topology_remove(CHFL_TOPOLOGY* const topology, uint64_t i) {
    CHECK_POINTER(topology);
    CHFL_ERROR_CATCH(
        topology->remove(checked_cast(i));
    )
}

extern "C" chfl_status chfl_topology_bonds_count(const CHFL_TOPOLOGY* const topology, uint64_t* nbonds) {
    CHECK_POINTER(topology);
    CHECK_POINTER(nbonds);
    CHFL_ERROR_CATCH(
        *nbonds = topology->bonds().size();
    )
}

extern "C" chfl_status chfl_topology_angles_count(const CHFL_TOPOLOGY* const topology, uint64_t* nangles) {
    CHECK_POINTER(topology);
    CHECK_POINTER(nangles);
    CHFL_ERROR_CATCH(
        *nangles = topology->angles().size();
    )
}

extern "C" chfl_status chfl_topology_dihedrals_count(const CHFL_TOPOLOGY* const topology, uint64_t* ndihedrals) {
    CHECK_POINTER(topology);
    CHECK_POINTER(ndihedrals);
    CHFL_ERROR_CATCH(
        *ndihedrals = topology->dihedrals().size();
    )
}

extern "C" chfl_status chfl_topology_impropers_count(const CHFL_TOPOLOGY* const topology, uint64_t* nimpropers) {
    CHECK_POINTER(topology);
    CHECK_POINTER(nimpropers);
    CHFL_ERROR_CATCH(
        *nimpropers = topology->impropers().size();
    )
}

#pragma intel optimization_level 2  /* Using -O3 with icc lead to partial copy of the bonds */
extern "C" chfl_status chfl_topology_bonds(const CHFL_TOPOLOGY* const topology, uint64_t (*data)[2], uint64_t nbonds) {
    CHECK_POINTER(topology);
    CHECK_POINTER(data);

    CHFL_ERROR_CATCH(
        if (nbonds != topology->bonds().size()) {
            set_last_error("wrong data size in function 'chfl_topology_bonds'.");
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
    CHECK_POINTER(topology);
    CHECK_POINTER(data);

    CHFL_ERROR_CATCH(
        if (nangles != topology->angles().size()) {
            set_last_error("wrong data size in function 'chfl_topology_angles'.");
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
    CHECK_POINTER(topology);
    CHECK_POINTER(data);

    CHFL_ERROR_CATCH(
        if (ndihedrals != topology->dihedrals().size()) {
            set_last_error("wrong data size in function 'chfl_topology_dihedrals'.");
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

#pragma intel optimization_level 2 /* Using -O3 with icc lead to partial copy of the dihedrals */
extern "C" chfl_status chfl_topology_impropers(const CHFL_TOPOLOGY* const topology, uint64_t (*data)[4], uint64_t nimpropers) {
    CHECK_POINTER(topology);
    CHECK_POINTER(data);

    CHFL_ERROR_CATCH(
        if (nimpropers != topology->impropers().size()) {
            set_last_error("wrong data size in function 'chfl_topology_impropers'.");
            return CHFL_MEMORY_ERROR;
        }

        auto& impropers = topology->impropers();
        for (size_t i=0; i<nimpropers; i++) {
            data[i][0] = static_cast<uint64_t>(impropers[i][0]);
            data[i][1] = static_cast<uint64_t>(impropers[i][1]);
            data[i][2] = static_cast<uint64_t>(impropers[i][2]);
            data[i][3] = static_cast<uint64_t>(impropers[i][3]);
        }
    )
}

extern "C" chfl_status chfl_topology_add_bond(CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j) {
    CHECK_POINTER(topology);
    CHFL_ERROR_CATCH(
        topology->add_bond(checked_cast(i), checked_cast(j));
    )
}

extern "C" chfl_status chfl_topology_remove_bond(CHFL_TOPOLOGY* const topology, uint64_t i, uint64_t j) {
    CHECK_POINTER(topology);
    CHFL_ERROR_CATCH(
        topology->remove_bond(checked_cast(i), checked_cast(j));
    )
}

extern "C" chfl_status chfl_topology_residues_count(const CHFL_TOPOLOGY* const topology, uint64_t* residues) {
    CHECK_POINTER(topology);
    CHECK_POINTER(residues);
    CHFL_ERROR_CATCH(
        *residues = topology->residues().size();
    )
}

extern "C" chfl_status chfl_topology_add_residue(CHFL_TOPOLOGY* const topology, const CHFL_RESIDUE* const residue) {
    CHECK_POINTER(topology);
    CHECK_POINTER(residue);
    CHFL_ERROR_CATCH(
        topology->add_residue(*residue);
    )
}

extern "C" chfl_status chfl_topology_residues_linked(
    const CHFL_TOPOLOGY* const topology, const CHFL_RESIDUE* const first,
    const CHFL_RESIDUE* const second, bool* result) {
    CHECK_POINTER(topology);
    CHECK_POINTER(first);
    CHECK_POINTER(second);
    CHECK_POINTER(result);
    CHFL_ERROR_CATCH(
        *result = topology->are_linked(*first, *second);
    )
}


extern "C" chfl_status chfl_topology_free(CHFL_TOPOLOGY* const topology) {
    delete topology;
    return CHFL_SUCCESS;
}
