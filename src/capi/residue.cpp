// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstring>

#include "chemfiles/capi/residue.h"
#include "chemfiles/capi.hpp"
#include "chemfiles/shared_allocator.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Topology.hpp"
using namespace chemfiles;

extern "C" CHFL_RESIDUE* chfl_residue(const char* name) {
    CHFL_RESIDUE* residue = nullptr;
    CHECK_POINTER_GOTO(name);
    CHFL_ERROR_GOTO(
        residue = shared_allocator::make_shared<Residue>(std::string(name));
    )
    return residue;
error:
    delete residue;
    return nullptr;
}

extern "C" CHFL_RESIDUE* chfl_residue_with_id(const char* name, uint64_t resid) {
    CHFL_RESIDUE* residue = nullptr;
    CHECK_POINTER_GOTO(name);
    CHFL_ERROR_GOTO(
        residue = shared_allocator::make_shared<Residue>(std::string(name), resid);
    )
    return residue;
error:
    delete residue;
    return nullptr;
}

extern "C" const CHFL_RESIDUE* chfl_residue_from_topology(const CHFL_TOPOLOGY* const topology, uint64_t i) {
    const CHFL_RESIDUE* residue = nullptr;
    CHECK_POINTER_GOTO(topology);
    CHFL_ERROR_GOTO(
        residue = shared_allocator::shared_ptr<Residue>(topology, &topology->residue(checked_cast(i)));
    )
    return residue;
error:
    delete residue;
    return nullptr;
}

extern "C" const CHFL_RESIDUE* chfl_residue_for_atom(const CHFL_TOPOLOGY* const topology, uint64_t i) {
    const CHFL_RESIDUE* residue = nullptr;
    CHECK_POINTER_GOTO(topology);
    CHFL_ERROR_GOTO(
        auto optional = topology->residue_for_atom(checked_cast(i));
        if (optional) {
            residue = shared_allocator::shared_ptr<Residue>(topology, &*optional);
        }
    )
    return residue;
error:
    delete residue;
    return nullptr;
}

extern "C" CHFL_RESIDUE* chfl_residue_copy(const CHFL_RESIDUE* const residue) {
    CHFL_RESIDUE* new_residue = nullptr;
    CHFL_ERROR_GOTO(
        new_residue = shared_allocator::make_shared<Residue>(*residue);
    )
    return new_residue;
error:
    delete new_residue;
    return nullptr;
}

extern "C" chfl_status chfl_residue_atoms_count(const CHFL_RESIDUE* const residue, uint64_t* size) {
    CHECK_POINTER(residue);
    CHECK_POINTER(size);
    CHFL_ERROR_CATCH(
        *size = residue->size();
    )
}

extern "C" chfl_status chfl_residue_atoms(const CHFL_RESIDUE* const residue, uint64_t atoms[], uint64_t natoms) {
    CHECK_POINTER(residue);
    CHECK_POINTER(atoms);
    CHFL_ERROR_CATCH(
        if (natoms != residue->size()) {
            set_last_error("wrong data size in function 'chfl_residue_atoms'.");
            return CHFL_MEMORY_ERROR;
        }

        size_t i = 0;
        for (size_t index: *residue) {
            atoms[i] = static_cast<uint64_t>(index);
            i++;
        }
    )
}

extern "C" chfl_status chfl_residue_id(const CHFL_RESIDUE* const residue, uint64_t* id) {
    CHECK_POINTER(residue);
    CHECK_POINTER(id);
    CHFL_ERROR_CATCH(
        try {
            *id = residue->id().value();
        } catch (const bad_optional_access&) {
            throw Error("this residue does not have a id");
        }
    )
}

extern "C" chfl_status chfl_residue_name(const CHFL_RESIDUE* const residue, char* name, uint64_t buffsize) {
    CHECK_POINTER(residue);
    CHECK_POINTER(name);
    CHFL_ERROR_CATCH(
        strncpy(name, residue->name().c_str(), checked_cast(buffsize) - 1);
        name[buffsize - 1] = '\0';
    )
}

extern "C" chfl_status chfl_residue_add_atom(CHFL_RESIDUE* const residue, uint64_t i) {
    CHECK_POINTER(residue);
    CHFL_ERROR_CATCH(
        residue->add_atom(checked_cast(i));
    )
}

extern "C" chfl_status chfl_residue_contains(const CHFL_RESIDUE* const residue, uint64_t i, bool* result) {
    CHECK_POINTER(residue);
    CHECK_POINTER(result);
    CHFL_ERROR_CATCH(
        *result = residue->contains(checked_cast(i));
    )
}

extern "C" chfl_status chfl_residue_set_property(CHFL_RESIDUE* const residue, const char* name, const CHFL_PROPERTY* const property) {
    CHECK_POINTER(residue);
    CHECK_POINTER(name);
    CHECK_POINTER(property);
    CHFL_ERROR_CATCH(
        residue->set(name, *property);
    )
}

extern "C" CHFL_PROPERTY* chfl_residue_get_property(const CHFL_RESIDUE* const residue, const char* name) {
    CHFL_PROPERTY* property = nullptr;
    CHECK_POINTER_GOTO(residue);
    CHECK_POINTER_GOTO(name);
    CHFL_ERROR_GOTO(
        auto residue_property = residue->get(name);
        if (residue_property) {
            property = new Property(*residue_property);
        } else {
            throw property_error("can not find a property named '{}' in this residue", name);
        }
    )
    return property;
error:
    delete property;
    return nullptr;
}

extern "C" chfl_status chfl_residue_free(const CHFL_RESIDUE* const residue) {
    CHFL_ERROR_CATCH(
        if (residue == nullptr) {
            return CHFL_SUCCESS;
        } else {
            shared_allocator::free(residue);
        }
    )
}
