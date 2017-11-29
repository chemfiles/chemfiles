// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_ATOM_HPP
#define CHEMFILES_ATOM_HPP

#include <string>

#include "chemfiles/exports.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/external/optional.hpp"

namespace chemfiles {

/// An `Atom` is a particle in the current `Frame`.
///
/// An atom stores atomic properties such as the atom name and type, the mass
/// and the charge. It also stores arbitrary property unsing the Property class.
/// Position and velocity are not stored in the `Atom` class, but in separated
/// arrays in the `Frame` class.
///
/// The atom name is usually an unique identifier ("H1", "C_a") while the atom
/// type will be shared between all particles of the same type: "H", "Ow",
/// "CH3".
class CHFL_EXPORT Atom final {
public:
    /// Create an atom with the given `name` and set the atom type to be the
    /// same as `name`.
    ///
    /// If the atom type exists in the periodic table, the atom mass is set to
    /// the matching value. This check is executed with case-insensitive atom
    /// type: `Na`, `NA`, `nA` and `na` all get the `Na` mass.
    ///
    /// @example{tests/doc/atom/atom.cpp}
    ///
    /// @param name atomic name for the new atom
    explicit Atom(std::string name = "");

    /// Create an atom from the given `name` and `type`
    ///
    /// If the atom type exists in the periodic table, the atom mass is set to
    /// the matching value. This check is executed with case-insensitive atom
    /// type: `Na`, `NA`, `nA` and `na` all get the `Na` mass.
    ///
    /// @example{tests/doc/atom/atom.cpp}
    ///
    /// @param name atomic name for the new atom
    /// @param type atomic type for the new atom
    Atom(std::string name, std::string type);

    ~Atom() = default;
    Atom(Atom&&) = default;
    Atom& operator=(Atom&&) = default;
    Atom(const Atom&) = default;
    Atom& operator=(const Atom&) = default;

    /// Get the atom name.
    ///
    /// @example{tests/doc/atom/name.cpp}
    const std::string& name() const { return name_; }

    /// Get the atom type.
    ///
    /// @example{tests/doc/atom/type.cpp}
    const std::string& type() const { return type_; }

    /// Get the atom mass.
    ///
    /// @example{tests/doc/atom/mass.cpp}
    double mass() const { return mass_; }

    /// Get the atom charge.
    ///
    /// @example{tests/doc/atom/charge.cpp}
    double charge() const { return charge_; }

    /// Set the atom name to `name`.
    ///
    /// @example{tests/doc/atom/name.cpp}
    void set_name(std::string name) { name_ = std::move(name); }

    /// Set the atom type to `type`.
    ///
    /// @example{tests/doc/atom/type.cpp}
    void set_type(std::string type) { type_ = std::move(type); }

    /// Set the atom mass to `mass`.
    ///
    /// @example{tests/doc/atom/mass.cpp}
    void set_mass(double mass) { mass_ = mass; }

    /// Set the atom charge to `charge`.
    ///
    /// @example{tests/doc/atom/mass.cpp}
    void set_charge(double charge) { charge_ = charge; }

    /// Try to get the full atomic name from the atom type.
    ///
    /// This function tries to get the full name corresponding to the current
    /// atom type: for example, the full name for `He` is `"Helium"`. If no name
    /// can be found, this function returns `nullopt`. This check is executed
    /// with case-insensitive atom type: `Na`, `NA`, `nA` and `na` all get the
    /// `Na` full name.
    ///
    /// @verbatim embed:rst:leading-slashes
    /// This function returna an :cpp:class:`chemfiles::optional` value that is
    /// close to C++17 ``std::optional``.
    /// @endverbatim
    ///
    /// @example{tests/doc/atom/full_name.cpp}
    optional<std::string> full_name() const;

    /// Try to get the Van der Waals radius from the atom type.
    ///
    /// This function tries to get the Van der Waals radius corresponding to the
    /// current atom type: for example, the radius for `He` is 1.4 A. If no
    /// radius can be found, this function returns `nullopt`. This check is
    /// executed with case-insensitive atom type: `Na`, `NA`, `nA` and `na` all
    /// get the `Na` radius.
    ///
    /// @verbatim embed:rst:leading-slashes
    /// This function returna an :cpp:class:`chemfiles::optional` value that is
    /// close to C++17 ``std::optional``.
    /// @endverbatim
    ///
    /// @example{tests/doc/atom/vdw_radius.cpp}
    optional<double> vdw_radius() const;

    /// Try to get the covalent radius from the atom type.
    ///
    /// This function tries to get the covalent radius corresponding to the
    /// current atom type: for example, the radius for `He` is 0.32 A. If no
    /// radius can be found, this function returns `nullopt`. This check is
    /// executed with case-insensitive atom type: `Na`, `NA`, `nA` and `na` all
    /// get the `Na` radius.
    ///
    /// @verbatim embed:rst:leading-slashes
    /// This function returna an :cpp:class:`chemfiles::optional` value that is
    /// close to C++17 ``std::optional``.
    /// @endverbatim
    ///
    /// @example{tests/doc/atom/covalent_radius.cpp}
    optional<double> covalent_radius() const;

    /// Try to get the atomic number from the atom type.
    ///
    /// This function tries to get the atomic number corresponding to the
    /// current atom type: for example, the atomic number for `He` is 2. If no
    /// atomic number can be found, this function returns `nullopt`. This check
    /// is executed with case-insensitive atom type: `Na`, `NA`, `nA` and `na`
    /// all get the `Na` atomic number.
    ///
    /// @verbatim embed:rst:leading-slashes
    /// This function returna an :cpp:class:`chemfiles::optional` value that is
    /// close to C++17 ``std::optional``.
    /// @endverbatim
    ///
    /// @example{tests/doc/atom/atomic_number.cpp}
    optional<uint64_t> atomic_number() const;

    /// Set an arbitrary `Property` for this atom with the given `name` and
    /// `value`. If a property with this name already exist, it is replaced with
    /// the new value.
    ///
    /// @example{tests/doc/atom/property.cpp}
    void set(std::string name, Property value);

    /// Get the `Property` with the given `name` for this atom if it exists.
    ///
    /// If no property with the given `name` is found, this function returns
    /// `nullopt`.
    ///
    /// @verbatim embed:rst:leading-slashes
    /// This function returna an :cpp:class:`chemfiles::optional` value that is
    /// close to C++17 ``std::optional``.
    /// @endverbatim
    ///
    /// @example{tests/doc/atom/property.cpp}
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

    friend bool operator==(const Atom& lhs, const Atom& rhs);
};

inline bool operator==(const Atom& lhs, const Atom& rhs) {
    return (lhs.name() == rhs.name() && lhs.type() == rhs.type() &&
            lhs.mass() == rhs.mass() && lhs.charge() == rhs.charge() &&
            lhs.properties_ == rhs.properties_);
}

inline bool operator!=(const Atom& lhs, const Atom& rhs) {
    return !(lhs == rhs);
}

} // namespace chemfiles

#endif
