// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/capi/property.h"
#include "chemfiles/capi.hpp"

#include "chemfiles/Property.hpp"
using namespace chemfiles;

static_assert(sizeof(chfl_property_kind) == sizeof(int), "Wrong size for chfl_property_kind enum");

extern "C" CHFL_PROPERTY* chfl_property_bool(bool value) {
    CHFL_PROPERTY* property = nullptr;
    CHFL_ERROR_GOTO(
        property = new Property(value);
    )
    return property;
error:
    delete property;
    return nullptr;
}

extern "C" CHFL_PROPERTY* chfl_property_double(double value) {
    CHFL_PROPERTY* property = nullptr;
    CHFL_ERROR_GOTO(
        property = new Property(value);
    )
    return property;
error:
    delete property;
    return nullptr;
}

extern "C" CHFL_PROPERTY* chfl_property_string(const char* value) {
    CHFL_PROPERTY* property = nullptr;
    CHFL_ERROR_GOTO(
        property = new Property(value);
    )
    return property;
error:
    delete property;
    return nullptr;
}

extern "C" CHFL_PROPERTY* chfl_property_vector3d(const chfl_vector3d value) {
    CHFL_PROPERTY* property = nullptr;
    CHFL_ERROR_GOTO(
        property = new Property(vector3d(value));
    )
    return property;
error:
    delete property;
    return nullptr;
}

extern "C" chfl_status chfl_property_get_kind(const CHFL_PROPERTY* const property, chfl_property_kind* kind) {
    CHECK_POINTER(property);
    CHECK_POINTER(kind);
    CHFL_ERROR_CATCH(
        *kind = static_cast<chfl_property_kind>(property->get_kind());
    )
}

extern "C" chfl_status chfl_property_get_bool(const CHFL_PROPERTY* const property, bool* value) {
    CHECK_POINTER(property);
    CHECK_POINTER(value);
    CHFL_ERROR_CATCH(
        *value = property->as_bool();
    )
}

extern "C" chfl_status chfl_property_get_double(const CHFL_PROPERTY* const property, double* value) {
    CHECK_POINTER(property);
    CHECK_POINTER(value);
    CHFL_ERROR_CATCH(
        *value = property->as_double();
    )
}

extern "C" chfl_status chfl_property_get_string(const CHFL_PROPERTY* const property, char* const buffer, uint64_t buffsize) {
    CHECK_POINTER(property);
    CHECK_POINTER(buffer);
    CHFL_ERROR_CATCH(
        const auto& string = property->as_string();
        strncpy(buffer, string.c_str(), checked_cast(buffsize) - 1);
        buffer[buffsize - 1] = '\0';
    )
}

extern "C" chfl_status chfl_property_get_vector3d(const CHFL_PROPERTY* const property, chfl_vector3d value) {
    CHECK_POINTER(property);
    CHECK_POINTER(value);
    CHFL_ERROR_CATCH(
        auto vector = property->as_vector3d();
        value[0] = vector[0];
        value[1] = vector[1];
        value[2] = vector[2];
    )
}

extern "C" chfl_status chfl_property_free(CHFL_PROPERTY* property) {
    delete property;
    return CHFL_SUCCESS;
}
