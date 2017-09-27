// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_ATOM_HPP
#define CHEMFILES_ATOM_HPP

#include <string>

#include "chemfiles/exports.hpp"
#include "chemfiles/optional.hpp"
#include "chemfiles/Property.hpp"

namespace chemfiles {

/// An `Atom` is a particle in the current `Frame`.
///
/// An atom stores the following atomic properties:
///
/// - atom name;
/// - atom type;
/// - atom mass;
/// - atom charge.
///
/// The atom name is usually an unique identifier ("H1", "C_a") while the atom
/// type will be shared between all particles of the same type: "H", "Ow",
/// "CH3".
class CHFL_EXPORT Atom {
public:
    /// Create an atom with the given `name` and set the atom `type` to be the
    /// same as `name`.
    explicit Atom(std::string name = "");
    /// Create an atom from the given `name` and `type`
    Atom(std::string name, std::string type);

    Atom(Atom&&) = default;
    Atom& operator=(Atom&&) = default;
    Atom(const Atom&) = default;
    Atom& operator=(const Atom&) = default;

    /// Get a const (non-modifiable) reference to the atom name
    const std::string& name() const { return name_; }
    /// Get a const (non-modifiable) reference to the atom type
    const std::string& type() const { return type_; }
    /// Get the atom mass
    double mass() const { return mass_; }
    /// Get the atom charge
    double charge() const { return charge_; }

    /// Set the atom type
    void set_type(std::string type) { type_ = std::move(type); }
    /// Set the atom name
    void set_name(std::string name) { name_ = std::move(name); }
    /// Set the atom mass
    void set_mass(double mass) { mass_ = mass; }
    /// Set the atom charge
    void set_charge(double charge) { charge_ = charge; }

    /// Try to get the full atomic name
    optional<std::string> full_name() const;
    /// Try to get the Van der Waals of the atom.
    optional<double> vdw_radius() const;
    /// Try to get the covalent radius of the atom.
    optional<double> covalent_radius() const;
    /// Try to get the atomic number of the atom.
    optional<uint64_t> atomic_number() const;

    /// Set an arbitrary property for this atom with the given `name` and
    /// `value`. If a property with this name already exist, it is replaced
    /// with the new value.
    void set(std::string name, Property value);

    /// Get the property with the given `name` for this atom if it exists.
    optional<const Property&> get(const std::string& name) const;
    
private:
    /// the atom name
    std::string name_;
    /// the atom type
    std::string type_;
    /// the atom mass
    double mass_ = 0;
    /// the atom charge
    double charge_ = 0;
    /// Additional properties of this atom
    property_map properties_;

    friend bool operator==(const Atom&, const Atom&);
};

inline bool operator==(const Atom& lhs, const Atom& rhs) {
    return (lhs.name() == rhs.name() && lhs.type() == rhs.type() &&
            lhs.mass() == rhs.mass() && lhs.charge() == rhs.charge() &&
            lhs.properties_ == rhs.properties_);
}

} // namespace chemfiles

#endif
