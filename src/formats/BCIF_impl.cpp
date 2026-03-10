// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <climits>
#include <string>
#include <vector>
#include <map>

#include <fmt/format.h>
#include "chemfiles/formats/BCIF_impl.hpp"
#include "chemfiles/Error.hpp"

namespace chemfiles {
namespace bcif_impl {

// =========================================================================
// Secondary structure constants
// =========================================================================

const char* HELIX_TYPES[10] = {
    "right-handed alpha helix",
    "right-handed omega helix",
    "right-handed pi helix",
    "right-handed gamma helix",
    "right-handed 3-10 helix",
    "left-handed alpha helix",
    "left-handed omega helix",
    "left-handed gamma helix",
    "2-7 ribbon/helix",
    "polyproline",
};

const char* PDB_BETA_SHEET = "extended";

// =========================================================================
// Secondary structure conversion
// =========================================================================

const char* bcif_to_pdb_secondary_structure(const std::string& conf_type_id) {
    if (conf_type_id == "HELX_RH_AL_P") return HELIX_TYPES[0];
    if (conf_type_id == "HELX_RH_OM_P") return HELIX_TYPES[1];
    if (conf_type_id == "HELX_RH_PI_P") return HELIX_TYPES[2];
    if (conf_type_id == "HELX_RH_GA_P") return HELIX_TYPES[3];
    if (conf_type_id == "HELX_RH_3T_P") return HELIX_TYPES[4];
    if (conf_type_id == "HELX_LH_AL_P") return HELIX_TYPES[5];
    if (conf_type_id == "HELX_LH_OM_P") return HELIX_TYPES[6];
    if (conf_type_id == "HELX_LH_GA_P") return HELIX_TYPES[7];
    if (conf_type_id == "HELX_RH_27_P") return HELIX_TYPES[8];
    if (conf_type_id == "HELX_LH_PP_P") return HELIX_TYPES[9];
    if (conf_type_id == "HELX_P") return HELIX_TYPES[0];
    if (conf_type_id == "STRN") return "extended";
    if (conf_type_id.find("TURN") == 0) return "turn";
    return nullptr;
}

std::string pdb_to_bcif_secondary_structure(const std::string& pdb_ss) {
    if (pdb_ss == "right-handed alpha helix") return "HELX_RH_AL_P";
    if (pdb_ss == "right-handed omega helix") return "HELX_RH_OM_P";
    if (pdb_ss == "right-handed pi helix") return "HELX_RH_PI_P";
    if (pdb_ss == "right-handed gamma helix") return "HELX_RH_GA_P";
    if (pdb_ss == "right-handed 3-10 helix") return "HELX_RH_3T_P";
    if (pdb_ss == "left-handed alpha helix") return "HELX_LH_AL_P";
    if (pdb_ss == "left-handed omega helix") return "HELX_RH_OM_P";
    if (pdb_ss == "left-handed gamma helix") return "HELX_LH_GA_P";
    if (pdb_ss == "2-7 ribbon/helix") return "HELX_RH_27_P";
    if (pdb_ss == "polyproline") return "HELX_LH_PP_P";
    if (pdb_ss == "extended") return "STRN";
    if (pdb_ss == "turn") return "TURN_TY1_P";
    return "HELX_P";
}

int32_t get_helix_class_from_pdb_ss(const std::string& pdb_ss) {
    for (int i = 0; i < 10; ++i) {
        if (pdb_ss == HELIX_TYPES[i]) {
            return i + 1;
        }
    }
    return 0;
}

// =========================================================================
// Bond order conversion
// =========================================================================

Bond::BondOrder parse_bond_order(const std::string& value_order) {
    if (value_order == "SING" || value_order == "sing") return Bond::SINGLE;
    if (value_order == "DOUB" || value_order == "doub") return Bond::DOUBLE;
    if (value_order == "TRIP" || value_order == "trip") return Bond::TRIPLE;
    if (value_order == "QUAD" || value_order == "quad") return Bond::QUADRUPLE;
    if (value_order == "AROM" || value_order == "arom") return Bond::AROMATIC;
    if (value_order.empty()) return Bond::UNKNOWN;
    return Bond::UNKNOWN;
}

std::string bond_order_to_string(Bond::BondOrder order) {
    switch (order) {
        case Bond::SINGLE: return "SING";
        case Bond::DOUBLE: return "DOUB";
        case Bond::TRIPLE: return "TRIP";
        case Bond::QUADRUPLE: return "QUAD";
        case Bond::AROMATIC: return "AROM";
        case Bond::UNKNOWN: return "?";
        default: return "?";
    }
}

// =========================================================================
// BCIF type code mapping
// =========================================================================

void get_int_type_str(const int32_t& type_code, std::string& type_str) {
    if (type_code == 1) type_str = "Int8";
    else if (type_code == 2) type_str = "Int16";
    else if (type_code == 4) type_str = "Int32";
    else if (type_code == 5) type_str = "Uint8";
    else if (type_code == 6) type_str = "Uint16";
    else if (type_code == 8) type_str = "Uint32";
    else if (type_code == 32) type_str = "Float32";
    else if (type_code == 33) type_str = "Float64";
    else throw FormatError(fmt::format("Unrocognized int type string <{}>", type_code));
}

// =========================================================================
// Float decoding utilities
// =========================================================================

void decode_float_column_float32(const uint8_t* bytes, const size_t& size, std::vector<double>& result) {
    result.resize(size / 4);
    for (size_t k = 0; k < result.size(); ++k) {
        uint32_t bits = bytes[k * 4]
            | (static_cast<uint32_t>(bytes[k * 4 + 1]) << 8)
            | (static_cast<uint32_t>(bytes[k * 4 + 2]) << 16)
            | (static_cast<uint32_t>(bytes[k * 4 + 3]) << 24);
        float f;
        std::memcpy(&f, &bits, sizeof(float));
        result[k] = static_cast<double>(f);
    }
}

void decode_float_column_float64(const uint8_t* bytes, const size_t& size, std::vector<double>& result) {
    result.resize(size / 8);
    for (size_t k = 0; k < result.size(); ++k) {
        uint64_t bits = static_cast<uint64_t>(bytes[k * 8])
            | (static_cast<uint64_t>(bytes[k * 8 + 1]) << 8)
            | (static_cast<uint64_t>(bytes[k * 8 + 2]) << 16)
            | (static_cast<uint64_t>(bytes[k * 8 + 3]) << 24)
            | (static_cast<uint64_t>(bytes[k * 8 + 4]) << 32)
            | (static_cast<uint64_t>(bytes[k * 8 + 5]) << 40)
            | (static_cast<uint64_t>(bytes[k * 8 + 6]) << 48)
            | (static_cast<uint64_t>(bytes[k * 8 + 7]) << 56);
        double d;
        std::memcpy(&d, &bits, sizeof(double));
        result[k] = d;
    }
}

// =========================================================================
// Integer packing utilities
// =========================================================================

void decode_integer_packing_1byte(const std::vector<uint8_t>& data,
    const int32_t& byte_count, const bool& is_unsigned, std::vector<int32_t>& result) {
    int32_t upper_limit = is_unsigned ? UINT8_MAX : INT8_MAX;
    int32_t lower_limit = is_unsigned ? 0 : INT8_MIN;

    size_t i = 0;
    while (i < data.size()) {
        int32_t value = 0;
        int32_t t;
        if (is_unsigned) {
            t = static_cast<int32_t>(data[i]);
        }
        else {
            t = static_cast<int32_t>(static_cast<int8_t>(data[i]));
        }

        if (is_unsigned) {
            while (t == upper_limit) {
                value += t;
                i++;
                if (i >= data.size()) break;
                t = is_unsigned ? static_cast<int32_t>(data[i])
                    : static_cast<int32_t>(static_cast<int8_t>(data[i]));
            }
        }
        else {
            while (t == upper_limit || t == lower_limit) {
                value += t;
                i++;
                if (i >= data.size()) break;
                t = static_cast<int32_t>(static_cast<int8_t>(data[i]));
            }
        }

        if (i < data.size()) {
            value += t;
            i++;
        }
        result.push_back(value);
    }
}

void decode_integer_packing_2bytes(const std::vector<uint8_t>& data,
    const int32_t& byte_count, const bool& is_unsigned, std::vector<int32_t>& result) {
    const int32_t upper_limit = is_unsigned ? UINT16_MAX : INT16_MAX;
    const int32_t lower_limit = is_unsigned ? 0 : INT16_MIN;

    size_t i = 0;
    while (i + 1 < data.size()) {
        int32_t value = 0;

        uint16_t raw_value = data[i] | (static_cast<uint16_t>(data[i + 1]) << 8);
        int32_t t;
        if (is_unsigned) {
            t = static_cast<int32_t>(raw_value);
        }
        else {
            t = static_cast<int32_t>(static_cast<int16_t>(raw_value));
        }

        if (is_unsigned) {
            while (t == upper_limit) {
                value += t;
                i += 2;
                if (i + 1 >= data.size()) break;
                raw_value = data[i] | (static_cast<uint16_t>(data[i + 1]) << 8);
                t = static_cast<int32_t>(raw_value);
            }
        }
        else {
            while (t == upper_limit || t == lower_limit) {
                value += t;
                i += 2;
                if (i + 1 >= data.size()) break;
                raw_value = data[i] | (static_cast<uint16_t>(data[i + 1]) << 8);
                t = static_cast<int32_t>(static_cast<int16_t>(raw_value));
            }
        }

        if (i + 1 < data.size()) {
            value += t;
            i += 2;
        }
        result.push_back(value);
    }
}

// =========================================================================
// Residue classification utilities
// =========================================================================

bool is_nucleotide(const std::string& residue_name) {
    return (residue_name == "A" || residue_name == "C" || residue_name == "G" || residue_name == "U" ||
           residue_name == "DA" || residue_name == "DC" || residue_name == "DG" || residue_name == "DT");
}

bool is_aminoacide(const std::string& residue_name) {
    return
        residue_name == "ALA" ||
        residue_name == "ARG" ||
        residue_name == "ASN" ||
        residue_name == "ASP" ||
        residue_name == "CYS" ||
        residue_name == "GLN" ||
        residue_name == "GLU" ||
        residue_name == "GLY" ||
        residue_name == "HIS" ||
        residue_name == "ILE" ||
        residue_name == "LEU" ||
        residue_name == "LYS" ||
        residue_name == "MET" ||
        residue_name == "PHE" ||
        residue_name == "PRO" ||
        residue_name == "SER" ||
        residue_name == "THR" ||
        residue_name == "TRP" ||
        residue_name == "TYR" ||
        residue_name == "VAL"
        ;
}

bool is_residue_forward_binder(const std::string& atomName) {
    return atomName == "C" || atomName == "O3'";
}

bool is_residue_backward_binder(const std::string& atomName) {
    return atomName == "N" || atomName == "P";
}

bool expect_implicit_inter_residue_bonding(const std::string& res_name) {
    return (is_nucleotide(res_name) || is_aminoacide(res_name));
}

bool is_placeholder_residue_data(const std::string& res_name, int32_t res_id, const std::string& chain_id) {
    return res_name == "UNK" && res_id == 1 && chain_id == "A";
}

// =========================================================================
// String encoding utility
// =========================================================================

void build_encode_string_data(const std::vector<std::string>& data, std::string& string_data) {
    std::vector<std::string> unique_strings;
    std::map<std::string, int32_t> string_to_index;
    for (const auto& str : data) {
        if (string_to_index.find(str) == string_to_index.end()) {
            string_to_index[str] = static_cast<int32_t>(unique_strings.size());
            unique_strings.push_back(str);
        }
    }
    for (const auto& str : unique_strings) {
        string_data += str;
    }
}

} // namespace bcif_impl
} // namespace chemfiles
