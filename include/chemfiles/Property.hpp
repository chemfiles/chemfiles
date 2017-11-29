// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_PROPERTY_HPP
#define CHEMFILES_PROPERTY_HPP

#include <string>
#include <unordered_map>

#include "chemfiles/exports.hpp"
#include "chemfiles/types.hpp"
#include "chemfiles/unreachable.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/external/optional.hpp"

namespace chemfiles {

/// This class holds the data used in properties by the `Atom` and the `Frame`
/// classes. A property can have various types: bool, double, string or
/// `Vector3D`.
class CHFL_EXPORT Property final {
public:
    /// Possible types holded by a property
    enum kind {
        BOOL = 0,
        DOUBLE = 1,
        STRING = 2,
        VECTOR3D = 3,
    };

    /// Create a property holding a boolean value.
    /// @example{tests/doc/property/bool.cpp}
    Property(bool value): kind_(BOOL), bool_(value) {}

    /// Create a property holding a double value.
    /// @example{tests/doc/property/double.cpp}
    Property(double value): kind_(DOUBLE), double_(value) {}

    /// Create a property holding a `Vector3D` value.
    /// @example{tests/doc/property/vector3d.cpp}
    Property(Vector3D value): kind_(VECTOR3D), vector3d_(value) {}

    /// Create a property holding a string value.
    /// @example{tests/doc/property/string.cpp}
    Property(std::string value): kind_(STRING), string_(std::move(value)) {}

    // ==== The following construtors are here to help with overloading
    // ==== resolution.
    /// Create a property holding a string value from a const char*.
    /// @example{tests/doc/property/string.cpp}
    Property(const char* value): kind_(STRING), string_(value) {}

    /// Create a property holding a double value from an int.
    /// @example{tests/doc/property/double.cpp}
    Property(int value): kind_(DOUBLE), double_(static_cast<double>(value)) {}

    /// Create a property holding a double value from a long.
    /// @example{tests/doc/property/double.cpp}
    Property(long value): kind_(DOUBLE), double_(static_cast<double>(value)) {}

    /// Create a property holding a double value from a long long.
    /// @example{tests/doc/property/double.cpp}
    Property(long long value): kind_(DOUBLE), double_(static_cast<double>(value)) {}

    /// Create a property holding a double value from an unsigned.
    /// @example{tests/doc/property/double.cpp}
    Property(unsigned value): kind_(DOUBLE), double_(static_cast<double>(value)) {}

    /// Create a property holding a double value from an unsigned long.
    /// @example{tests/doc/property/double.cpp}
    Property(unsigned long value): kind_(DOUBLE), double_(static_cast<double>(value)) {}

    /// Create a property holding a double value from an unsigned long long.
    /// @example{tests/doc/property/double.cpp}
    Property(unsigned long long value): kind_(DOUBLE), double_(static_cast<double>(value)) {}

    Property(const Property& other): Property(false) {
        *this = other;
    }

    Property(Property&& other): Property(false) {
        *this = std::move(other);
    }

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

    Property& operator=(Property&& other) {
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
            new (&this->string_) std::string(std::move(other.string_));
            break;
        case VECTOR3D:
            new (&this->vector3d_) Vector3D(other.vector3d_);
            break;
        }
        return *this;
    }

    ~Property() {
        if (kind_ == STRING) {
            using std::string;
            this->string_.~string();
        }
    }

    /// Get the kind of property, *i.e.* the type of the holded value
    ///
    /// @example{tests/doc/property/get_kind.cpp}
    kind get_kind() const {
        return this->kind_;
    }

    /// Get the boolean value stored in this Property
    ///
    /// @example{tests/doc/property/as_bool.cpp}
    ///
    /// @throws PropertyError if this property does not hold a boolean value
    bool as_bool() const;

    /// Get the double value stored in this Property
    ///
    /// @example{tests/doc/property/as_double.cpp}
    ///
    /// @throws PropertyError if this property does not hold a double value
    double as_double() const;

    /// Get the Vector3D value stored in this Property
    ///
    /// @example{tests/doc/property/as_vector3d.cpp}
    ///
    /// @throws PropertyError if this property does not hold a Vector3D value
    Vector3D as_vector3d() const;

    /// Get the string value stored in this Property
    ///
    /// @example{tests/doc/property/as_string.cpp}
    ///
    /// @throws PropertyError if this property does not hold a string value
    const std::string& as_string() const;

private:
    /// Get the kind name as a string
    std::string kind_as_string() const;

    kind kind_;
    union {
        bool bool_;
        std::string string_;
        double double_;
        Vector3D vector3d_;
    };

    friend bool operator==(const Property& lhs, const Property& rhs);
};

inline bool operator==(const Property& lhs, const Property& rhs) {
    if (lhs.kind_ != rhs.kind_) {
        return false;
    }
    switch (lhs.kind_) {
    case Property::BOOL:
        return lhs.bool_ == rhs.bool_;
    case Property::STRING:
        return lhs.string_ == rhs.string_;
    case Property::DOUBLE:
        return lhs.double_ == rhs.double_;
    case Property::VECTOR3D:
        return lhs.vector3d_ == rhs.vector3d_;
    }
    unreachable();
}

inline bool operator!=(const Property& lhs, const Property& rhs) {
    return !(lhs == rhs);
}

/// A property map for inclusion in a Frame or an Atom.
class property_map final {
public:
    property_map() = default;

    /// Set an arbitrary property with the given `name` and `value`. If a
    /// property with this name already exist, it is replaced with the new
    /// value.
    void set(std::string name, Property value);

    /// Get the property with the given `name` if it exists.
    optional<const Property&> get(const std::string& name) const;

private:
    std::unordered_map<std::string, Property> data_;
    friend bool operator==(const property_map& lhs, const property_map& rhs);
};

inline bool operator==(const property_map& lhs, const property_map& rhs) {
    return lhs.data_ == rhs.data_;
}

inline bool operator!=(const property_map& lhs, const property_map& rhs) {
    return !(lhs == rhs);
}

} // namespace chemfiles

#endif
