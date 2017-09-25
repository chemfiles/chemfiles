// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Property.hpp"
#include "chemfiles/utils.hpp"
using namespace chemfiles;

bool Property::as_bool() const {
    if (kind_ == BOOL) {
        return bool_;
    } else {
        throw PropertyError(
            "Tried to use 'as_bool' on a " + kind_as_string() + " property"
        );
    }
}

double Property::as_double() const {
    if (kind_ == DOUBLE) {
        return double_;
    } else {
        throw PropertyError(
            "Tried to use 'as_double' on a " + kind_as_string() + " property"
        );
    }
}

Vector3D Property::as_vector3d() const {
    if (kind_ == VECTOR3D) {
        return vector3d_;
    } else {
        throw PropertyError(
            "Tried to use 'as_vector3d' on a " + kind_as_string() + " property"
        );
    }
}

const std::string& Property::as_string() const {
    if (kind_ == STRING) {
        return string_;
    } else {
        throw PropertyError(
            "Tried to use 'as_string' on a " + kind_as_string() + " property"
        );
    }
}

std::string Property::kind_as_string() const {
    switch (this->kind_) {
    case BOOL:
        return "bool";
    case DOUBLE:
        return "double";
    case STRING:
        return "string";
    case VECTOR3D:
        return "Vector3D";
    }
    unreachable();
}
