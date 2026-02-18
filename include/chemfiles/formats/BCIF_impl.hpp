// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_BCIF_IMPL_HPP
#define CHEMFILES_FORMAT_BCIF_IMPL_HPP

#include <cstdint>
#include <string>

#include "chemfiles/Connectivity.hpp"

namespace chemfiles {
namespace bcif_impl {

/// See http://www.wwpdb.org/documentation/file-format-content/format23/sect5.html
/// for definitions of helix types
extern const char* HELIX_TYPES[10];
extern const char* PDB_BETA_SHEET;

/// Convert BCIF/mmCIF conf_type_id to PDB-style secondary structure name.
/// Returns nullptr if the conf_type_id is not recognized.
const char* bcif_to_pdb_secondary_structure(const std::string& conf_type_id);

/// Convert PDB-style secondary structure name back to BCIF conf_type_id.
std::string pdb_to_bcif_secondary_structure(const std::string& pdb_ss);

/// Get helix class (1-10) from PDB secondary structure name, or 0 if not a helix.
int32_t get_helix_class_from_pdb_ss(const std::string& pdb_ss);

/// Convert mmCIF bond order string to Bond::BondOrder enum.
Bond::BondOrder parse_bond_order(const std::string& value_order);

/// Convert Bond::BondOrder enum to mmCIF bond order string.
std::string bond_order_to_string(Bond::BondOrder order);

} // namespace bcif_impl
} // namespace chemfiles

#endif
