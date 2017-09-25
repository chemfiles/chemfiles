// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_PROPERTY_HPP
#define CHEMFILES_PROPERTY_HPP

#include <string>

#include "chemfiles/exports.hpp"
#include "chemfiles/types.hpp"
#include "chemfiles/Error.hpp"

namespace chemfiles {

/// This class holds the data used in properties by the `Atom` and the `Frame`
/// classes. A property can have various types: bool, double, string or
/// `Vector3D`.
class CHFL_EXPORT Property final {
public:
    /// Create a property holding a boolean value.
    Property(bool value): kind_(BOOL), bool_(value) {}
    /// Create a property holding a double value.
    Property(double value): kind_(DOUBLE), double_(value) {}
    /// Create a property holding a `Vector3D` value.
    Property(Vector3D value): kind_(VECTOR3D), vector3d_(value) {}
    /// Create a property holding a string value.
    Property(std::string value): kind_(STRING), string_(value) {}

    // ==== The following construtors are here to help with overloading
    // ==== resolution.
    /// Create a property holding a string value from a const char*.
    Property(const char* value): kind_(STRING), string_(value) {}
    /// Create a property holding a double value from an int.
    Property(int value): kind_(DOUBLE), double_(static_cast<double>(value)) {}
    /// Create a property holding a double value from a long.
    Property(long value): kind_(DOUBLE), double_(static_cast<double>(value)) {}
    /// Create a property holding a double value from a long long.
    Property(long long value): kind_(DOUBLE), double_(static_cast<double>(value)) {}
    /// Create a property holding a double value from an unsigned.
    Property(unsigned value): kind_(DOUBLE), double_(static_cast<double>(value)) {}
    /// Create a property holding a double value from an unsigned long.
    Property(unsigned long value): kind_(DOUBLE), double_(static_cast<double>(value)) {}
    /// Create a property holding a double value from an unsigned long long.
    Property(unsigned long long value): kind_(DOUBLE), double_(static_cast<double>(value)) {}

    /// The copy constructor for Property
    Property(const Property& other): Property(false) {
        *this = other;
    }

    /// The move constructor for Property
    Property(Property&& other): Property(false) {
        *this = std::move(other);
    }

    /// The copy assignement operator for Property
    Property& operator=(const Property& other) {
        this->~Property();
        this->kind_ = other.kind_;
        switch (this->kind_) {
        case BOOL:
            new(&this->bool_) bool(other.bool_);
            break;
        case DOUBLE:
            new (&this->double_) double(other.double_);
            break;
        case STRING:
            new (&this->string_) std::string(other.string_);
            break;
        case VECTOR3D:
            new (&this->vector3d_) Vector3D(other.vector3d_);
            break;
        }
        return *this;
    }

    /// The move assignement operator for Property
    Property& operator=(Property&& other) {
        this->~Property();
        this->kind_ = other.kind_;
        switch (this->kind_) {
        case BOOL:
            new(&this->bool_) bool(std::move(other.bool_));
            break;
        case DOUBLE:
            new (&this->double_) double(std::move(other.double_));
            break;
        case STRING:
            new (&this->string_) std::string(std::move(other.string_));
            break;
        case VECTOR3D:
            new (&this->vector3d_) Vector3D(std::move(other.vector3d_));
            break;
        }
        return *this;
    }

    /// The destructor for Property
    ~Property() {
        if (kind_ == STRING) {
            using std::string;
            this->string_.~string();
        }
    }

    /// Get the boolean value stored in this Property
    ///
    /// @throws PropertyError if this property does not hold a boolean value
    bool as_bool() const;

    /// Get the double value stored in this Property
    ///
    /// @throws PropertyError if this property does not hold a double value
    double as_double() const;

    /// Get the Vector3D value stored in this Property
    ///
    /// @throws PropertyError if this property does not hold a Vector3D value
    Vector3D as_vector3d() const;

    /// Get the string value stored in this Property
    ///
    /// @throws PropertyError if this property does not hold a string value
    const std::string& as_string() const;

private:
    /// Get the kind name as a string
    std::string kind_as_string() const;

    enum {
        BOOL,
        STRING,
        DOUBLE,
        VECTOR3D,
    } kind_;

    union {
        bool bool_;
        std::string string_;
        double double_;
        Vector3D vector3d_;
    };
};

} // namespace chemfiles

#endif
