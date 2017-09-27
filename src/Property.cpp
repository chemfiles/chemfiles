// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Property.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/utils.hpp"
using namespace chemfiles;

bool Property::as_bool() const {
    if (kind_ == BOOL) {
        return bool_;
    } else {
        throw property_error(
            "can not call 'as_bool' on a {} property", kind_as_string()
        );
    }
}

double Property::as_double() const {
    if (kind_ == DOUBLE) {
        return double_;
    } else {
        throw property_error(
            "can not call 'as_double' on a {} property", kind_as_string()
        );
    }
}

Vector3D Property::as_vector3d() const {
    if (kind_ == VECTOR3D) {
        return vector3d_;
    } else {
        throw property_error(
            "can not call 'as_vector3d' on a {} property", kind_as_string()
        );
    }
}

const std::string& Property::as_string() const {
    if (kind_ == STRING) {
        return string_;
    } else {
        throw property_error(
            "can not call 'as_string' on a {} property", kind_as_string()
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


void property_map::set(std::string name, Property value) {
    // We can not move value here, because we might need it later. C++17 solves
    // this with insert_or_assign.
    auto inserted = data_.emplace(std::move(name), value);
    if (!inserted.second) {
        inserted.first->second = std::move(value);
    }
}

optional<const Property&> property_map::get(const std::string& name) const {
    auto property = data_.find(name);
    if (property != data_.end()) {
        return property->second;
    } else {
        return nullopt;
    }
}
