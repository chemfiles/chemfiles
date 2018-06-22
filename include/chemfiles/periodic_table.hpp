// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

// !!!! AUTO-GENERATED FILE !!!! Do not edit. See elements.py for the code.
// The data comes from Blue Obelisk's data repository at the svn repository:
// http://svn.code.sf.net/p/bodr/code/trunk/bodr

#ifndef CHEMFILES_PERIODIC_HPP
#define CHEMFILES_PERIODIC_HPP

#include <unordered_map>
#include <string>
#include <cstdint>
#include "chemfiles/external/optional.hpp"

namespace chemfiles {

//! Storing basic atomic data
struct AtomicData {
    //! Atomic number
    optional<uint64_t> number;
    //! Full name
    optional<std::string> full_name;
    //! Mass in atomic units
    optional<double> mass;
    //! Default charge in units of e
    optional<double> charge;
    //! Covalent radius in Angstrom
    optional<double> covalent_radius;
    //! Van der Waals radius in Angstrom
    optional<double> vdw_radius;
};

using atomic_data_map = std::unordered_map<std::string, AtomicData>;

/// Find the data for a given element with its `type`.
///
/// If the `type` string is one or two characters long, it is normalized to
/// the standard element case (first character in upper case, second one in
/// lower case).

// This is implemented in Atom.cpp
optional<const AtomicData&> find_in_periodic_table(const std::string& type);


extern const atomic_data_map PERIODIC_TABLE;

} // namespace chemfiles

#endif
