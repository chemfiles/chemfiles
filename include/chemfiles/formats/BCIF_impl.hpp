// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_BCIF_IMPL_HPP
#define CHEMFILES_FORMAT_BCIF_IMPL_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <map>

#include "chemfiles/Connectivity.hpp"

namespace chemfiles {
namespace bcif_impl {

// =========================================================================
// Secondary structure constants and conversion
// =========================================================================

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

// =========================================================================
// Bond order conversion
// =========================================================================

/// Convert mmCIF bond order string to Bond::BondOrder enum.
Bond::BondOrder parse_bond_order(const std::string& value_order);

/// Convert Bond::BondOrder enum to mmCIF bond order string.
std::string bond_order_to_string(Bond::BondOrder order);

// =========================================================================
// BCIF type code mapping
// =========================================================================

/// Map numeric type codes to type strings.
/// Codes: 1=Int8, 2=Int16, 4=Int32, 5=Uint8, 6=Uint16, 8=Uint32, 32=Float32, 33=Float64
void get_int_type_str(const int32_t& type_code, std::string& type_str);

// =========================================================================
// Float decoding utilities
// =========================================================================

/// Decode little-endian Float32 binary data to double vector.
void decode_float_column_float32(const uint8_t* bytes, const size_t& size, std::vector<double>& result);

/// Decode little-endian Float64 binary data to double vector.
void decode_float_column_float64(const uint8_t* bytes, const size_t& size, std::vector<double>& result);

// =========================================================================
// Integer packing utilities
// =========================================================================

/// Decode integer packing for 8-bit values with overflow handling.
void decode_integer_packing_1byte(const std::vector<uint8_t>& data,
    const int32_t& byte_count, const bool& is_unsigned, std::vector<int32_t>& result);

/// Decode integer packing for 16-bit values with overflow handling.
void decode_integer_packing_2bytes(const std::vector<uint8_t>& data,
    const int32_t& byte_count, const bool& is_unsigned, std::vector<int32_t>& result);

// =========================================================================
// Residue classification utilities
// =========================================================================

/// Check if a residue name corresponds to a nucleotide (RNA or DNA).
bool is_nucleotide(const std::string& residue_name);

/// Check if a residue name corresponds to a standard amino acid.
bool is_aminoacide(const std::string& residue_name);

/// Check if an atom name is a forward inter-residue binder (C or O3').
bool is_residue_forward_binder(const std::string& atomName);

/// Check if an atom name is a backward inter-residue binder (N or P).
bool is_residue_backward_binder(const std::string& atomName);

/// Check if a residue type expects implicit inter-residue bonding.
bool expect_implicit_inter_residue_bonding(const std::string& res_name);

/// Check if residue data matches the placeholder pattern (UNK, id=1, chain=A).
bool is_placeholder_residue_data(const std::string& res_name, int32_t res_id, const std::string& chain_id);

// =========================================================================
// String encoding utility
// =========================================================================

/// Build string pool from a vector of strings (concatenate unique strings).
void build_encode_string_data(const std::vector<std::string>& data, std::string& string_data);

} // namespace bcif_impl
} // namespace chemfiles

#endif
