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

namespace chemfiles {

//! Storing basic elemental data: mass, colvalent and Van der Waals radii
struct ElementData {
    //! Atomic number
    const uint64_t number;
    //! Full name
    const std::string name;
    //! Mass in atomic units
    const double mass;
    //! Covalent radius in Angstrom
    const double covalent_radius;
    //! Van der Waals radius in Angstrom
    const double vdw_radius;
};

extern const std::unordered_map<std::string, ElementData> PERIODIC_TABLE;

} // namespace chemfiles

#endif
