// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <string>
#include "chemfiles/formats/BCIF_impl.hpp"

namespace chemfiles {
namespace bcif_impl {

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

const char* bcif_to_pdb_secondary_structure(const std::string& conf_type_id) {
    // BCIF/mmCIF conf_type_id values from mmCIF dictionary
    // See: http://mmcif.wwpdb.org/dictionaries/mmcif_pdbx_v50.dic/Items/_struct_conf.conf_type_id.html

    if (conf_type_id == "HELX_RH_AL_P") return HELIX_TYPES[0];  // right-handed alpha
    if (conf_type_id == "HELX_RH_OM_P") return HELIX_TYPES[1];  // right-handed omega
    if (conf_type_id == "HELX_RH_PI_P") return HELIX_TYPES[2];  // right-handed pi
    if (conf_type_id == "HELX_RH_GA_P") return HELIX_TYPES[3];  // right-handed gamma
    if (conf_type_id == "HELX_RH_3T_P") return HELIX_TYPES[4];  // right-handed 3-10
    if (conf_type_id == "HELX_LH_AL_P") return HELIX_TYPES[5];  // left-handed alpha
    if (conf_type_id == "HELX_LH_OM_P") return HELIX_TYPES[6];  // left-handed omega
    if (conf_type_id == "HELX_LH_GA_P") return HELIX_TYPES[7];  // left-handed gamma
    if (conf_type_id == "HELX_RH_27_P") return HELIX_TYPES[8];  // 2-7 ribbon/helix
    if (conf_type_id == "HELX_LH_PP_P") return HELIX_TYPES[9];  // polyproline

    // HELX_P is a generic helix - default to alpha helix
    if (conf_type_id == "HELX_P") return HELIX_TYPES[0];

    // Beta strands/sheets
    if (conf_type_id == "STRN") return "extended";

    // Turns (various types all map to "turn" in PDB)
    if (conf_type_id.find("TURN") == 0) return "turn";

    // If unknown, return nullptr
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
    if (pdb_ss == "turn") return "TURN_TY1_P";  // Default turn type

    // If unknown, return generic helix
    return "HELX_P";
}

int32_t get_helix_class_from_pdb_ss(const std::string& pdb_ss) {
    // HELIX_TYPES array maps indices 0-9 to helix classes 1-10
    for (int i = 0; i < 10; ++i) {
        if (pdb_ss == HELIX_TYPES[i]) {
            return i + 1;  // Return class 1-10
        }
    }
    return 0;  // Not a helix
}

Bond::BondOrder parse_bond_order(const std::string& value_order) {
    if (value_order == "SING" || value_order == "sing") return Bond::SINGLE;
    if (value_order == "DOUB" || value_order == "doub") return Bond::DOUBLE;
    if (value_order == "TRIP" || value_order == "trip") return Bond::TRIPLE;
    if (value_order == "QUAD" || value_order == "quad") return Bond::QUADRUPLE;
    if (value_order == "AROM" || value_order == "arom") return Bond::AROMATIC;
    // Default to UNKNOWN for empty or unknown bond orders
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

} // namespace bcif_impl
} // namespace chemfiles
