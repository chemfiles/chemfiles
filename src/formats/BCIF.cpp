// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cmath>

#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <unordered_set>
#include <thread>

// Include msgpack headers
#include <msgpack.hpp>
#include <msgpack/sbuffer.hpp>
#include <msgpack/pack.hpp>

#include "chemfiles/types.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/unreachable.hpp"
#include "chemfiles/external/span.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/File.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/FormatMetadata.hpp"

#include "chemfiles/files/MemoryBuffer.hpp"
#include "chemfiles/formats/BCIF.hpp"

using namespace ::chemfiles; 

template<> const FormatMetadata& ::chemfiles::format_metadata<BCIFFormat>() {
    static FormatMetadata metadata;
    metadata.name = "BCIF";
    metadata.extension = ".bcif";
    metadata.description = "BinaryCIF (binary mmCIF) format";
    metadata.reference = "https://github.com/dsehnal/BinaryCIF";

    metadata.read = true;
    metadata.write = true; 
    metadata.memory = true;

    metadata.positions = true;
    metadata.velocities = false;
    metadata.unit_cell = true;
    metadata.atoms = true;
    metadata.bonds = true;
    metadata.residues = true;
    return metadata;
}

// Hash function for std::pair<std::string, std::string> used in ChemCompMap
namespace std {
    template<>
    struct hash<std::pair<std::string, std::string>> {
        size_t operator()(const std::pair<std::string, std::string>& p) const {
            auto h1 = std::hash<std::string>{}(p.first);
            auto h2 = std::hash<std::string>{}(p.second);
            // Combine hash values using a simple approach
            return h1 ^ (h2 << 1);
        }
    };
    template<>
    struct hash<std::pair<size_t, size_t>> {
        size_t operator()(const std::pair<size_t, size_t>& p) const {
            auto h1 = std::hash<size_t>{}(p.first);
            auto h2 = std::hash<size_t>{}(p.second);
            // Combine hash values using a simple approach
            return h1 ^ (h2 << 1);
        }
    };
}
namespace chemfiles {

    struct BCIFFormat::BCIFData {
        // Atom site data
        std::vector<double> atom_x;
        std::vector<double> atom_y;
        std::vector<double> atom_z;
        std::vector<std::string> atom_type_symbol;
        std::vector<std::string> atom_label;        // label_atom_id (primary)
        std::vector<std::string> auth_atom_label;   // auth_atom_id (fallback)
        std::vector<int32_t> atom_id;

        // Residue data (from _atom_site category)
        std::vector<std::string> residue_name;      // label_comp_id
        std::vector<int32_t> residue_id;            // label_seq_id (primary)
        std::vector<std::string> chain_id;          // label_asym_id (primary)
        std::vector<std::string> insertion_code;    // pdbx_PDB_ins_code

        // Author-provided identifiers (stored for round-trip writing)
        std::vector<int32_t> auth_residue_id;       // auth_seq_id
        std::vector<std::string> auth_chain_id;     // auth_asym_id

        // Cell data
        double cell_length_a = 0.0;
        double cell_length_b = 0.0;
        double cell_length_c = 0.0;
        double cell_angle_alpha = 90.0;
        double cell_angle_beta = 90.0;
        double cell_angle_gamma = 90.0;

        // Entry data
        std::string entry_id;

        // Model tracking
        size_t num_models = 1;

        // Secondary structure data
        using ChainNameResId = std::pair<std::string, uint32_t>;
        std::map<ChainNameResId, const char*> secondary_structure_map;

        // Bond data (from _chem_comp_bond category)
        // Component-level bond definitions (template bonds for residue types)
        struct ChemCompBond {
            std::string comp_id;              // Residue name
            std::string atom_id_1;            // First atom name
            std::string atom_id_2;            // Second atom name
            std::string value_order;          // Bond order (sing, doub, trip, etc.)
            std::string pdbx_aromatic_flag;   // For round-trip
            std::string pdbx_stereo_config;   // For round-trip
            int32_t pdbx_ordinal = 0;         // For round-trip
        };
        std::vector<ChemCompBond> chem_comp_bonds;
        using AtomName = std::string;
        using ResName = std::string;
        using BondOrder = std::string;
        using ChemCompMapKey = std::pair<ResName, AtomName>;
        using ChemCompMapValue = std::pair<AtomName, BondOrder>;
        using ChemCompMap = std::unordered_multimap<ChemCompMapKey, ChemCompMapValue>;
        ChemCompMap chem_comp_bonds_map;

        // Bond data (from _struct_conn category)
        // Instance-specific bonds (inter-residue, metal coordination, disulfide, etc.)
        struct StructConn {
            struct LinkPartner
            {
                std::string label_asym_id;      // Chain ID 
                std::string label_comp_id;      // Residue name 
                int32_t label_seq_id;           // Residue ID
                std::string label_atom_id;      // Atom name 
            };
            std::string conn_type_id;             // Type of connection (covale, disulf, metalc, etc.)
            LinkPartner ptnr1, ptnr2;
            std::string pdbx_value_order;         // Bond order
        };
        std::vector<StructConn> struct_conns;
        
        using ChaineName = std::string;
        using SeqId = int32_t;
        using StructConnMapKey = std::tuple<ChaineName, ResName, SeqId, AtomName>;
        using StructConnMap = std::map<StructConnMapKey, StructConn*>;
        StructConnMap struct_conn_map;
    };
    void BCIFFormat::BCIFDataDtor::operator()(BCIFFormat::BCIFData* ptr) noexcept{
        delete ptr;
    };

}


namespace
{

    // See http://www.wwpdb.org/documentation/file-format-content/format23/sect5.html
    // for definitions of helix types
    static const char* HELIX_TYPES[10] = {
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
    static const char* PDB_BETA_SHEET = "extended";

    // Convert BCIF/mmCIF conf_type_id to PDB-style secondary structure name
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

        // If unknown, return the original value
        return nullptr;
    }

    // Convert PDB-style secondary structure name back to BCIF conf_type_id
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

    // Get helix class (1-10) from PDB secondary structure name, or 0 if not a helix
    int32_t get_helix_class_from_pdb_ss(const std::string& pdb_ss) {
        // HELIX_TYPES array maps indices 0-9 to helix classes 1-10
        for (int i = 0; i < 10; ++i) {
            if (pdb_ss == HELIX_TYPES[i]) {
                return i + 1;  // Return class 1-10
            }
        }
        return 0;  // Not a helix
    }

    // Convert mmCIF bond order string to Bond::BondOrder enum
    Bond::BondOrder parse_bond_order(const std::string& value_order) {
        if (value_order == "SING" || value_order == "sing") return Bond::SINGLE;
        if (value_order == "DOUB" || value_order == "doub") return Bond::DOUBLE;
        if (value_order == "TRIP" || value_order == "trip") return Bond::TRIPLE;
        if (value_order == "QUAD" || value_order == "quad") return Bond::QUADRUPLE;
        if (value_order == "AROM" || value_order == "arom") return Bond::AROMATIC;
        // Default to SINGLE for empty or unknown bond orders
        if (value_order.empty()) return Bond::UNKNOWN;
        return Bond::UNKNOWN;
    }

    // Convert Bond::BondOrder enum to mmCIF bond order string
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
}

// =============================================================================
// Free functions in msgpack namespace for BCIF decoding
// These functions must be in the msgpack namespace to properly access msgpack types
// =============================================================================

namespace msgpack {
    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {

        using BCIFData = BCIFFormat::BCIFData;

        // Debug helper to print msgpack structure
        void print_msgpack_structure(const msgpack::object& obj, int indent = 0, const std::string& key = "");

        // BCIF decoder function declarations
        std::vector<int32_t> decode_byte_array(const msgpack::object & data);
        std::vector<int32_t> decode_integer_packing(const std::vector<uint8_t>&data,
            int32_t byte_count, bool is_unsigned);
        std::vector<int32_t> decode_delta(const std::vector<int32_t>&data, int32_t origin);
        std::vector<double> decode_fixed_point(const std::vector<int32_t>&data, double factor);
        std::vector<int32_t> decode_run_length(const std::vector<int32_t>& data, int32_t src_size);
        std::vector<std::string> decode_string_array(const msgpack::object & encoding, const msgpack::object & data);

        std::vector<int32_t> decode_mask(const msgpack::object & mask_obj);
        void decode_column(const msgpack::object & column, std::vector<double>&);
        void decode_column(const msgpack::object & column, std::vector<int32_t>&);
        void decode_column(const msgpack::object & column, std::vector<std::string>&);

        void parse_atom_site(const msgpack::object & category, BCIFData & data);
        void parse_cell(const msgpack::object & category, BCIFData & data);
        void parse_struct_conf(const msgpack::object & category, BCIFData & data);
        void parse_struct_sheet_range(const msgpack::object & category, BCIFData & data);
        void parse_chem_comp_bond(const msgpack::object & category, BCIFData & data);
        void parse_struct_conn(const msgpack::object & category, BCIFData & data);
        void parse_category(const std::string & category_name, const msgpack::object & category,
            BCIFData & data);
        void parse_data_block(const msgpack::object & block, BCIFData & data);

        // BCIF encoder function declarations (for writing)
        std::vector<uint8_t> encode_byte_array_float64(const std::vector<double>& data);
        std::vector<uint8_t> encode_byte_array_int32(const std::vector<int32_t>& data);
        std::vector<int32_t> encode_delta(const std::vector<int32_t>& data, int32_t& origin_out);
        std::vector<int32_t> encode_run_length(const std::vector<int32_t>& data);
        std::vector<uint8_t> encode_integer_packing(const std::vector<int32_t>& data,
            int32_t& byte_count_out, bool& is_unsigned_out);
        std::pair<std::vector<int32_t>, std::vector<uint8_t>> encode_string_array(
            const std::vector<std::string>& strings);

        // Helper functions for encoding BCIF structure
        void encode_atom_site_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame);
        void encode_cell_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame);
        void encode_struct_conf_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame);
        void encode_struct_sheet_range_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame);
        void encode_chem_comp_bond_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame);
        void encode_struct_conn_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame);
        void encode_float_column(msgpack::packer<msgpack::sbuffer>& pk,
            const std::string& column_name, const std::vector<double>& data);
        void encode_string_column(msgpack::packer<msgpack::sbuffer>& pk,
            const std::string& column_name, const std::vector<std::string>& data);
        void encode_integer_column(msgpack::packer<msgpack::sbuffer>& pk,
            const std::string& column_name, const std::vector<int32_t>& data);

        // =============================================================================
        // Free function implementations in msgpack namespace
        // =============================================================================

        void parse_data_block(const msgpack::object & block, BCIFData & data) {

            if (block.type != msgpack::type::MAP) {
                throw format_error("the BCIF data block must be a map");
            }

            // Get categories map
            msgpack::object categories_obj;
            bool found_categories = false;

            auto block_map = block.via.map;
            for (uint32_t i = 0; i < block_map.size; ++i) {
                std::string key;
                block_map.ptr[i].key.convert(key);
                if (key == "categories") {
                    categories_obj = block_map.ptr[i].val;
                    found_categories = true;
                    break;
                }
            }

            if (!found_categories || categories_obj.type != msgpack::type::ARRAY) {
                throw format_error("the BCIF data block must contain a categories array");
            }

            // Parse each category
            auto categories = categories_obj.via.array;
            for (uint32_t i = 0; i < categories.size; ++i) {
                if (categories.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                // Get category name
                std::string category_name;
                auto cat_map = categories.ptr[i].via.map;
                for (uint32_t j = 0; j < cat_map.size; ++j) {
                    std::string key;
                    cat_map.ptr[j].key.convert(key);
                    if (key == "name") {
                        cat_map.ptr[j].val.convert(category_name);
                        break;
                    }
                }

                if (category_name.empty()) {
                    continue;
                }

                parse_category(category_name, categories.ptr[i], data);
            }
        }

        void parse_category(const std::string & category_name, const msgpack::object & category,
            BCIFData & data) {
            if (category_name == "_atom_site") {
            /*
            * TMP : taken care of using tasks
                parse_atom_site(category, data);

                // Fallback to auth fields when label fields are invalid
                // Some BCIF files (e.g., 1aga) have label_seq_id = -1 for all atoms
                bool label_seq_invalid = !data.residue_id.empty() &&
                    std::all_of(data.residue_id.begin(), data.residue_id.end(),
                        [](int32_t id) { return id < 0; });

                if (label_seq_invalid && !data.auth_residue_id.empty()) {
                    // Use auth_seq_id when label_seq_id is invalid
                    data.residue_id = data.auth_residue_id;
                }

                // Similarly for chain IDs (though less common)
                bool label_chain_invalid = !data.chain_id.empty() &&
                    std::all_of(data.chain_id.begin(), data.chain_id.end(),
                        [](const std::string& s) { return s.empty() || s == "?" || s == "."; });

                if (label_chain_invalid && !data.auth_chain_id.empty()) {
                    // Use auth_asym_id when label_asym_id is invalid
                    data.chain_id = data.auth_chain_id;
                }
            */
            }
            else if (category_name == "_cell") {
                parse_cell(category, data);
            }
            else if (category_name == "_struct_conf") {
                parse_struct_conf(category, data);
            }
            else if (category_name == "_struct_sheet_range") {
                parse_struct_sheet_range(category, data);
            }
            else if (category_name == "_chem_comp_bond") {
                parse_chem_comp_bond(category, data);
            }
            else if (category_name == "_struct_conn") {
                parse_struct_conn(category, data);
            }
        }

        std::vector<int32_t> apply_mask(const std::vector<int32_t>& data,
                                                      const std::vector<int32_t>& mask) {
            // Expand data array using mask
            // Mask: 0=present (use data), 1/2=missing (use default)
            std::vector<int32_t> result;
            result.reserve(mask.size());

            size_t data_idx = 0;
            for (size_t i = 0; i < mask.size(); ++i) {
                if (mask[i] == 0) {
                    // Value is present
                    if (data_idx < data.size()) {
                        result.push_back(data[data_idx]);
                        data_idx++;
                    } else {
                        result.push_back(0);  // Safety fallback
                    }
                } else {
                    // Value is missing (1=not present, 2=unknown)
                    result.push_back(-1);  // Use -1 for missing integer values
                }
            }

            return result;
        }

        std::vector<std::string> apply_mask(const std::vector<std::string>& data,
                                                         const std::vector<int32_t>& mask) {
            // Expand data array using mask
            // Mask: 0=present (use data), 1/2=missing (use "?")
            std::vector<std::string> result;
            result.reserve(mask.size());

            size_t data_idx = 0;
            for (size_t i = 0; i < mask.size(); ++i) {
                if (mask[i] == 0) {
                    // Value is present
                    if (data_idx < data.size()) {
                        result.push_back(data[data_idx]);
                        data_idx++;
                    } else {
                        result.push_back("?");  // Safety fallback
                    }
                } else {
                    // Value is missing (1=not present, 2=unknown)
                    result.push_back("?");  // Use "?" for missing string values
                }
            }

            return result;
        }
        namespace
        {
            inline void get_name_and_data(msgpack::object_map& col_map, std::string& column_name, msgpack::object& data_obj, bool& found_data, msgpack::object& mask_obj, bool& has_mask)
            {
                for (uint32_t j = 0; j < col_map.size; ++j) {
                    std::string key;
                    col_map.ptr[j].key.convert(key);
                    if (key == "name") {
                        col_map.ptr[j].val.convert(column_name);
                    }
                    else if (key == "data") {
                        data_obj = col_map.ptr[j].val;
                        found_data = true;
                    }
                    else if (key == "mask") {
                        if (col_map.ptr[j].val.type != msgpack::type::NIL) {
                            mask_obj = col_map.ptr[j].val;
                            has_mask = true;
                        }
                    }
                }
            }
            inline void parse_atom_site_column(const msgpack::object& mask_obj, const bool& has_mask, const msgpack::object& data_obj, const std::string& column_name, BCIFData& data)
            {
                // Decode mask if present
                std::vector<int32_t> mask;
                if (has_mask) {
                    mask = decode_mask(mask_obj);
                }

                // Decode based on column name
                if (column_name == "Cartn_x") {
                    decode_column(data_obj, data.atom_x);
                    // Masks for float columns would need special handling
                }
                else if (column_name == "Cartn_y") {
                    decode_column(data_obj, data.atom_y);
                }
                else if (column_name == "Cartn_z") {
                    decode_column(data_obj, data.atom_z);
                }
                else if (column_name == "type_symbol") {
                    decode_column(data_obj, data.atom_type_symbol);
                    if (has_mask && !mask.empty()) {
                        data.atom_type_symbol = apply_mask(data.atom_type_symbol, mask);
                    }
                }
                else if (column_name == "label_atom_id") {
                    decode_column(data_obj, data.atom_label);
                    if (has_mask && !mask.empty()) {
                        data.atom_label = apply_mask(data.atom_label, mask);
                    }
                }
                else if (column_name == "auth_atom_id") {
                    decode_column(data_obj, data.auth_atom_label);
                    if (has_mask && !mask.empty()) {
                        data.auth_atom_label = apply_mask(data.auth_atom_label, mask);
                    }
                }
                else if (column_name == "id") {
                    decode_column(data_obj, data.atom_id);
                    if (has_mask && !mask.empty()) {
                        data.atom_id = apply_mask(data.atom_id, mask);
                    }
                }
                // Residue information
                else if (column_name == "label_comp_id") {
                    decode_column(data_obj, data.residue_name);
                    if (has_mask && !mask.empty()) {
                        data.residue_name = apply_mask(data.residue_name, mask);
                    }
                }
                else if (column_name == "label_seq_id") {
                    // Use label_seq_id as primary residue ID (matches mmCIF implementation)
                    decode_column(data_obj, data.residue_id);
                    if (has_mask && !mask.empty()) {
                        data.residue_id = apply_mask(data.residue_id, mask);
                    }
                }
                else if (column_name == "auth_seq_id") {
                    // Store auth_seq_id for round-trip writing
                    decode_column(data_obj, data.auth_residue_id);
                    if (has_mask && !mask.empty()) {
                        data.auth_residue_id = apply_mask(data.auth_residue_id, mask);
                    }
                }
                else if (column_name == "label_asym_id") {
                    // Use label_asym_id as primary chain ID (matches mmCIF implementation)
                    decode_column(data_obj, data.chain_id);
                    if (has_mask && !mask.empty()) {
                        data.chain_id = apply_mask(data.chain_id, mask);
                    }
                }
                else if (column_name == "auth_asym_id") {
                    // Store auth_asym_id for round-trip writing
                    decode_column(data_obj, data.auth_chain_id);
                    if (has_mask && !mask.empty()) {
                        data.auth_chain_id = apply_mask(data.auth_chain_id, mask);
                    }
                }
                else if (column_name == "pdbx_PDB_ins_code") {
                    decode_column(data_obj, data.insertion_code);
                    if (has_mask && !mask.empty()) {
                        data.insertion_code = apply_mask(data.insertion_code, mask);
                    }
                }
            }

        }
        void parse_atom_site(const msgpack::object & category, BCIFData & data) {
            // Get columns array from category
            msgpack::object columns_obj;
            bool found_columns = false;

            auto cat_map = category.via.map;
            for (uint32_t i = 0; i < cat_map.size; ++i) {
                std::string key;
                cat_map.ptr[i].key.convert(key);
                if (key == "columns") {
                    columns_obj = cat_map.ptr[i].val;
                    found_columns = true;
                    break;
                }
            }

            if (!found_columns || columns_obj.type != msgpack::type::ARRAY) {
                return;
            }

            // Parse each column
            auto columns = columns_obj.via.array;
            for (uint32_t i = 0; i < columns.size; ++i) {
                if (columns.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                // Get column name, mask, and data
                std::string column_name;
                msgpack::object data_obj;
                msgpack::object mask_obj;
                bool found_data = false;
                bool has_mask = false;
                get_name_and_data(columns.ptr[i].via.map, column_name, data_obj, found_data, mask_obj, has_mask);

                if (!found_data || column_name.empty()) {
                    continue;
                }
                parse_atom_site_column(mask_obj, has_mask, data_obj, column_name, data);
            }
        }
        void parse_cell(const msgpack::object & category, BCIFData & data) {
            // Get columns array from category
            msgpack::object columns_obj;
            bool found_columns = false;

            auto cat_map = category.via.map;
            for (uint32_t i = 0; i < cat_map.size; ++i) {
                std::string key;
                cat_map.ptr[i].key.convert(key);
                if (key == "columns") {
                    columns_obj = cat_map.ptr[i].val;
                    found_columns = true;
                    break;
                }
            }

            if (!found_columns || columns_obj.type != msgpack::type::ARRAY) {
                return;
            }

            // Parse each column
            auto columns = columns_obj.via.array;
            for (uint32_t i = 0; i < columns.size; ++i) {
                if (columns.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                // Get column name and data
                std::string column_name;
                msgpack::object data_obj;
                bool found_data = false;

                auto col_map = columns.ptr[i].via.map;
                for (uint32_t j = 0; j < col_map.size; ++j) {
                    std::string key;
                    col_map.ptr[j].key.convert(key);
                    if (key == "name") {
                        col_map.ptr[j].val.convert(column_name);
                    }
                    else if (key == "data") {
                        data_obj = col_map.ptr[j].val;
                        found_data = true;
                    }
                }

                if (!found_data || column_name.empty()) {
                    continue;
                }

                // Try to decode cell parameters - handle both encoded and direct values
                std::vector<double> values;

                // Check if data is a direct value (number) or encoded (map with encoding)
                if (data_obj.type == msgpack::type::POSITIVE_INTEGER ||
                    data_obj.type == msgpack::type::NEGATIVE_INTEGER ||
                    data_obj.type == msgpack::type::FLOAT32 ||
                    data_obj.type == msgpack::type::FLOAT64) {
                    // Direct numeric value
                    double val;
                    data_obj.convert(val);
                    values.push_back(val);
                } else if (data_obj.type == msgpack::type::ARRAY) {
                    // Direct array of values
                    auto arr = data_obj.via.array;
                    for (uint32_t k = 0; k < arr.size; ++k) {
                        double val;
                        arr.ptr[k].convert(val);
                        values.push_back(val);
                    }
                } else {
                    // Encoded data (map with encoding field)
                     decode_column(data_obj, values);
                }

                if (values.empty()) {
                    continue;
                }

                // Cell parameters are single values
                if (column_name == "length_a") {
                    data.cell_length_a = values[0];
                }
                else if (column_name == "length_b") {
                    data.cell_length_b = values[0];
                }
                else if (column_name == "length_c") {
                    data.cell_length_c = values[0];
                }
                else if (column_name == "angle_alpha") {
                    data.cell_angle_alpha = values[0];
                }
                else if (column_name == "angle_beta") {
                    data.cell_angle_beta = values[0];
                }
                else if (column_name == "angle_gamma") {
                    data.cell_angle_gamma = values[0];
                }
            }
        }

        namespace
        {
            struct StructConfData
            {
                std::vector<std::string> conf_type_id;
                std::vector<int32_t> pdbx_PDB_helix_class;  // Helix type (1-10)
                std::vector<std::string> beg_label_asym_id;
                std::vector<int32_t> beg_label_seq_id;
                std::vector<std::string> end_label_asym_id;
                std::vector<int32_t> end_label_seq_id;
                std::vector<std::string> beg_auth_asym_id;
                std::vector<int32_t> beg_auth_seq_id;
                std::vector<std::string> end_auth_asym_id;
                std::vector<int32_t> end_auth_seq_id;

            };
            inline void parse_struct_conf_column(const msgpack::object& mask_obj, const bool& has_mask, const msgpack::object& data_obj, const std::string& column_name, StructConfData& struct_data)
            {

                // Decode mask if present
                std::vector<int32_t> mask;
                if (has_mask) {
                    mask = decode_mask(mask_obj);
                }

                // Parse the columns we care about
                if (column_name == "conf_type_id") {
                    decode_column(data_obj, struct_data.conf_type_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.conf_type_id = apply_mask(struct_data.conf_type_id, mask);
                    }
                }
                else if (column_name == "pdbx_PDB_helix_class") {
                    decode_column(data_obj, struct_data.pdbx_PDB_helix_class);
                    if (has_mask && !mask.empty()) {
                        struct_data.pdbx_PDB_helix_class = apply_mask(struct_data.pdbx_PDB_helix_class, mask);
                    }
                }
                else if (column_name == "beg_label_asym_id") {
                    decode_column(data_obj, struct_data.beg_label_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.beg_label_asym_id = apply_mask(struct_data.beg_label_asym_id, mask);
                    }
                }
                else if (column_name == "beg_label_seq_id") {
                    decode_column(data_obj, struct_data.beg_label_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.beg_label_seq_id = apply_mask(struct_data.beg_label_seq_id, mask);
                    }
                }
                else if (column_name == "end_label_asym_id") {
                    decode_column(data_obj, struct_data.end_label_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.end_label_asym_id = apply_mask(struct_data.end_label_asym_id, mask);
                    }
                }
                else if (column_name == "end_label_seq_id") {
                    decode_column(data_obj, struct_data.end_label_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.end_label_seq_id = apply_mask(struct_data.end_label_seq_id, mask);
                    }
                }
                else if (column_name == "beg_auth_asym_id") {
                    decode_column(data_obj, struct_data.beg_auth_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.beg_auth_asym_id = apply_mask(struct_data.beg_auth_asym_id, mask);
                    }
                }
                else if (column_name == "beg_auth_seq_id") {
                    decode_column(data_obj, struct_data.beg_auth_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.beg_auth_seq_id = apply_mask(struct_data.beg_auth_seq_id, mask);
                    }
                }
                else if (column_name == "end_auth_asym_id") {
                    decode_column(data_obj, struct_data.end_auth_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.end_auth_asym_id = apply_mask(struct_data.end_auth_asym_id, mask);
                    }
                }
                else if (column_name == "end_auth_seq_id") {
                    decode_column(data_obj, struct_data.end_auth_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.end_auth_seq_id = apply_mask(struct_data.end_auth_seq_id, mask);
                    }
                }
            }
            inline void parse_struct_conf_build_ss_map(const StructConfData& struct_data, BCIFFormat::BCIFData& data)
            {
                size_t num_entries = std::min({
                    struct_data.conf_type_id.size(),
                    struct_data.beg_label_asym_id.size(),
                    struct_data.beg_label_seq_id.size(),
                    struct_data.end_label_asym_id.size(),
                    struct_data.end_label_seq_id.size()
                    });

                for (size_t i = 0; i < num_entries; ++i) {
                    // Use label fields, with fallback to auth fields if needed
                    std::string beg_chain = struct_data.beg_label_asym_id[i];
                    int32_t beg_resid = struct_data.beg_label_seq_id[i];
                    std::string end_chain = struct_data.end_label_asym_id[i];
                    int32_t end_resid = struct_data.end_label_seq_id[i];

                    // Fallback to auth fields if label fields are invalid
                    if ((beg_chain.empty() || beg_chain == "?" || beg_chain == ".") &&
                        i < struct_data.beg_auth_asym_id.size()) {
                        beg_chain = struct_data.beg_auth_asym_id[i];
                    }
                    if (beg_resid < 0 && i < struct_data.beg_auth_seq_id.size()) {
                        beg_resid = struct_data.beg_auth_seq_id[i];
                    }
                    if ((end_chain.empty() || end_chain == "?" || end_chain == ".") &&
                        i < struct_data.end_auth_asym_id.size()) {
                        end_chain = struct_data.end_auth_asym_id[i];
                    }
                    if (end_resid < 0 && i < struct_data.end_auth_seq_id.size()) {
                        end_resid = struct_data.end_auth_seq_id[i];
                    }

                    // Store the secondary structure range
                    // Determine the secondary structure type
                    const char* pdb_ss_type = nullptr;

                    // Check if this is a helix with pdbx_PDB_helix_class specified
                    if (i < struct_data.pdbx_PDB_helix_class.size() && struct_data.pdbx_PDB_helix_class[i] >= 1 && struct_data.pdbx_PDB_helix_class[i] <= 10) {
                        // Use helix class to get specific helix type (class 1-10 maps to HELIX_TYPES[0-9])
                        pdb_ss_type = HELIX_TYPES[struct_data.pdbx_PDB_helix_class[i] - 1];
                    }
                    else {
                        // Fall back to converting conf_type_id to PDB-style name
                        pdb_ss_type = bcif_to_pdb_secondary_structure(struct_data.conf_type_id[i]);
                    }


                    if (beg_chain == end_chain)
                        for (size_t resid = beg_resid; resid <= static_cast<size_t>(end_resid); resid++)
                        {
                            data.secondary_structure_map[BCIFFormat::BCIFData::ChainNameResId(beg_chain, static_cast<uint32_t>(resid))] = pdb_ss_type;
                        }
                }
            }
        }
        void parse_struct_conf(const msgpack::object & category, BCIFData & data) {
            // Get columns array from category
            msgpack::object columns_obj;
            bool found_columns = false;

            auto cat_map = category.via.map;
            for (uint32_t i = 0; i < cat_map.size; ++i) {
                std::string key;
                cat_map.ptr[i].key.convert(key);
                if (key == "columns") {
                    columns_obj = cat_map.ptr[i].val;
                    found_columns = true;
                    break;
                }
            }

            if (!found_columns || columns_obj.type != msgpack::type::ARRAY) {
                return;
            }

            // Storage for the columns we need
            StructConfData struct_data;

            // Parse each column
            auto columns = columns_obj.via.array;
            for (uint32_t i = 0; i < columns.size; ++i) {
                if (columns.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                // Get column name and data
                std::string column_name;
                msgpack::object data_obj;
                msgpack::object mask_obj;
                bool found_data = false;
                bool has_mask = false;
                get_name_and_data(columns.ptr[i].via.map, column_name, data_obj, found_data, mask_obj, has_mask);

                if (!found_data || column_name.empty()) {
                    continue;
                }
                parse_struct_conf_column(mask_obj, has_mask, data_obj, column_name, struct_data);
            }
            parse_struct_conf_build_ss_map(struct_data, data);
        }

        namespace {
            struct StructSheetData
            {
                std::vector<std::string> beg_label_asym_id;
                std::vector<int32_t> beg_label_seq_id;
                std::vector<std::string> end_label_asym_id;
                std::vector<int32_t> end_label_seq_id;
                std::vector<std::string> beg_auth_asym_id;
                std::vector<int32_t> beg_auth_seq_id;
                std::vector<std::string> end_auth_asym_id;
                std::vector<int32_t> end_auth_seq_id;

            };
            inline void parse_struct_sheet_range_data(const msgpack::object& data_obj, const msgpack::object& mask_obj, const bool& has_mask, const std::string& column_name, StructSheetData& struct_data)
            {

                // Decode mask if present
                std::vector<int32_t> mask;
                if (has_mask) {
                    mask = decode_mask(mask_obj);
                }

                // Parse the columns we care about
                if (column_name == "beg_label_asym_id") {
                    decode_column(data_obj, struct_data.beg_label_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.beg_label_asym_id = apply_mask(struct_data.beg_label_asym_id, mask);
                    }
                }
                else if (column_name == "beg_label_seq_id") {
                    decode_column(data_obj, struct_data.beg_label_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.beg_label_seq_id = apply_mask(struct_data.beg_label_seq_id, mask);
                    }
                }
                else if (column_name == "end_label_asym_id") {
                    decode_column(data_obj, struct_data.end_label_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.end_label_asym_id = apply_mask(struct_data.end_label_asym_id, mask);
                    }
                }
                else if (column_name == "end_label_seq_id") {
                    decode_column(data_obj, struct_data.end_label_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.end_label_seq_id = apply_mask(struct_data.end_label_seq_id, mask);
                    }
                }
                else if (column_name == "beg_auth_asym_id") {
                    decode_column(data_obj, struct_data.beg_auth_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.beg_auth_asym_id = apply_mask(struct_data.beg_auth_asym_id, mask);
                    }
                }
                else if (column_name == "beg_auth_seq_id") {
                    decode_column(data_obj, struct_data.beg_auth_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.beg_auth_seq_id = apply_mask(struct_data.beg_auth_seq_id, mask);
                    }
                }
                else if (column_name == "end_auth_asym_id") {
                    decode_column(data_obj, struct_data.end_auth_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.end_auth_asym_id = apply_mask(struct_data.end_auth_asym_id, mask);
                    }
                }
                else if (column_name == "end_auth_seq_id") {
                    decode_column(data_obj, struct_data.end_auth_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.end_auth_seq_id = apply_mask(struct_data.end_auth_seq_id, mask);
                    }
                }
            }
            inline void parse_struct_sheet_range_build_ss_map(const StructSheetData& struct_data, BCIFFormat::BCIFData& data)
            {
                size_t num_entries = std::min({
                    struct_data.beg_label_asym_id.size(),
                    struct_data.beg_label_seq_id.size(),
                    struct_data.end_label_asym_id.size(),
                    struct_data.end_label_seq_id.size()
                    });

                for (size_t i = 0; i < num_entries; ++i) {
                    // Use label fields, with fallback to auth fields if needed
                    std::string beg_chain = struct_data.beg_label_asym_id[i];
                    int32_t beg_resid = struct_data.beg_label_seq_id[i];
                    std::string end_chain = struct_data.end_label_asym_id[i];
                    int32_t end_resid = struct_data.end_label_seq_id[i];

                    // Fallback to auth fields if label fields are invalid
                    if ((beg_chain.empty() || beg_chain == "?" || beg_chain == ".") &&
                        i < struct_data.beg_auth_asym_id.size()) {
                        beg_chain = struct_data.beg_auth_asym_id[i];
                    }
                    if (beg_resid < 0 && i < struct_data.beg_auth_seq_id.size()) {
                        beg_resid = struct_data.beg_auth_seq_id[i];
                    }
                    if ((end_chain.empty() || end_chain == "?" || end_chain == ".") &&
                        i < struct_data.end_auth_asym_id.size()) {
                        end_chain = struct_data.end_auth_asym_id[i];
                    }
                    if (end_resid < 0 && i < struct_data.end_auth_seq_id.size()) {
                        end_resid = struct_data.end_auth_seq_id[i];
                    }

                    if (beg_chain == end_chain)
                        for (size_t resid = beg_resid; resid <= static_cast<size_t>(end_resid); resid++)
                        {
                            data.secondary_structure_map[std::make_pair(beg_chain, static_cast<uint32_t>(resid))] = PDB_BETA_SHEET;
                        }
                }
            }
        }
        void parse_struct_sheet_range(const msgpack::object & category, BCIFData & data) {
            // Get columns array from category
            msgpack::object columns_obj;
            bool found_columns = false;

            auto cat_map = category.via.map;
            for (uint32_t i = 0; i < cat_map.size; ++i) {
                std::string key;
                cat_map.ptr[i].key.convert(key);
                if (key == "columns") {
                    columns_obj = cat_map.ptr[i].val;
                    found_columns = true;
                    break;
                }
            }

            if (!found_columns || columns_obj.type != msgpack::type::ARRAY) {
                return;
            }

            // Storage for the columns we need
            StructSheetData struct_data;

            // Parse each column
            auto columns = columns_obj.via.array;
            for (uint32_t i = 0; i < columns.size; ++i) {
                if (columns.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                // Get column name and data
                std::string column_name;
                msgpack::object data_obj;
                msgpack::object mask_obj;
                bool found_data = false;
                bool has_mask = false;

                get_name_and_data(columns.ptr[i].via.map, column_name, data_obj, found_data, mask_obj, has_mask);

                if (!found_data || column_name.empty()) {
                    continue;
                }
                parse_struct_sheet_range_data(data_obj, mask_obj, has_mask, column_name, struct_data);
            }
            parse_struct_sheet_range_build_ss_map(struct_data, data);
        }
        
        namespace
        {
            struct ChemCompBondData
            {
                std::vector<std::string> comp_id;
                std::vector<std::string> atom_id_1;
                std::vector<std::string> atom_id_2;
                std::vector<std::string> value_order;
                std::vector<std::string> pdbx_aromatic_flag;
                std::vector<std::string> pdbx_stereo_config;
                std::vector<int32_t> pdbx_ordinal;
            };
            inline void parse_chem_comp_bond_column(const std::string& column_name, const msgpack::object& data_obj, const msgpack::object& mask_obj, const bool& has_mask, ChemCompBondData& bond_data)
            {
                // Decode mask if present
                std::vector<int32_t> mask;
                if (has_mask) {
                    mask = decode_mask(mask_obj);
                }

                // Parse the columns relevant to us
                if (column_name == "comp_id") {
                    decode_column(data_obj, bond_data.comp_id);
                    if (has_mask && !mask.empty()) {
                        bond_data.comp_id = apply_mask(bond_data.comp_id, mask);
                    }
                }
                else if (column_name == "atom_id_1") {
                    decode_column(data_obj, bond_data.atom_id_1);
                    if (has_mask && !mask.empty()) {
                        bond_data.atom_id_1 = apply_mask(bond_data.atom_id_1, mask);
                    }
                }
                else if (column_name == "atom_id_2") {
                    decode_column(data_obj, bond_data.atom_id_2);
                    if (has_mask && !mask.empty()) {
                        bond_data.atom_id_2 = apply_mask(bond_data.atom_id_2, mask);
                    }
                }
                else if (column_name == "value_order") {
                    decode_column(data_obj, bond_data.value_order);
                    if (has_mask && !mask.empty()) {
                        bond_data.value_order = apply_mask(bond_data.value_order, mask);
                    }
                }
                else if (column_name == "pdbx_aromatic_flag") {
                    decode_column(data_obj, bond_data.pdbx_aromatic_flag);
                    if (has_mask && !mask.empty()) {
                        bond_data.pdbx_aromatic_flag = apply_mask(bond_data.pdbx_aromatic_flag, mask);
                    }
                }
                else if (column_name == "pdbx_stereo_config") {
                    decode_column(data_obj, bond_data.pdbx_stereo_config);
                    if (has_mask && !mask.empty()) {
                        bond_data.pdbx_stereo_config = apply_mask(bond_data.pdbx_stereo_config, mask);
                    }
                }
                else if (column_name == "pdbx_ordinal") {
                    decode_column(data_obj, bond_data.pdbx_ordinal);
                    if (has_mask && !mask.empty()) {
                        bond_data.pdbx_ordinal = apply_mask(bond_data.pdbx_ordinal, mask);
                    }
                }
            }

            inline void generate_bonds(const ChemCompBondData& bond_data, std::vector<BCIFFormat::BCIFData::ChemCompBond>& out, BCIFFormat::BCIFData::ChemCompMap& map)
            {
                // Build the bond list
                size_t num_entries = bond_data.comp_id.size();

                for (size_t i = 0; i < num_entries; ++i) {
                    out.push_back({});
                    BCIFData::ChemCompBond& bond = out.back();
                    bond.comp_id = (i < bond_data.comp_id.size()) ? bond_data.comp_id[i] : "";
                    bond.atom_id_1 = (i < bond_data.atom_id_1.size()) ? bond_data.atom_id_1[i] : "";
                    bond.atom_id_2 = (i < bond_data.atom_id_2.size()) ? bond_data.atom_id_2[i] : "";
                    bond.value_order = (i < bond_data.value_order.size()) ? bond_data.value_order[i] : "";
                    bond.pdbx_aromatic_flag = (i < bond_data.pdbx_aromatic_flag.size()) ? bond_data.pdbx_aromatic_flag[i] : "";
                    bond.pdbx_stereo_config = (i < bond_data.pdbx_stereo_config.size()) ? bond_data.pdbx_stereo_config[i] : "";
                    bond.pdbx_ordinal = (i < bond_data.pdbx_ordinal.size()) ? bond_data.pdbx_ordinal[i] : 0;

                    // Insert bond into map (bidirectional: atom1->atom2 and atom2->atom1)
                    BCIFFormat::BCIFData::ChemCompMapKey key1{ bond.comp_id, bond.atom_id_1 };
                    BCIFFormat::BCIFData::ChemCompMapValue val1{ bond.atom_id_2, bond.value_order };
                    BCIFFormat::BCIFData::ChemCompMapKey key2{ bond.comp_id, bond.atom_id_2 };
                    BCIFFormat::BCIFData::ChemCompMapValue val2{ bond.atom_id_1, bond.value_order };

                    map.insert({ key1, val1 });
                    map.insert({ key2, val2 });
                }

            }
        }
        void parse_chem_comp_bond(const msgpack::object & category, BCIFData & data) {
            // Get columns array from category
            msgpack::object columns_obj;
            bool found_columns = false;

            auto cat_map = category.via.map;
            for (uint32_t i = 0; i < cat_map.size; ++i) {
                std::string key;
                cat_map.ptr[i].key.convert(key);
                if (key == "columns") {
                    columns_obj = cat_map.ptr[i].val;
                    found_columns = true;
                    break;
                }
            }

            if (!found_columns || columns_obj.type != msgpack::type::ARRAY) {
                return;
            }

            ChemCompBondData bond_data;

            // Parse each column
            auto columns = columns_obj.via.array;
            for (uint32_t i = 0; i < columns.size; ++i) {
                if (columns.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                // Get column name and data
                std::string column_name;
                msgpack::object data_obj;
                msgpack::object mask_obj;
                bool found_data = false;
                bool has_mask = false;
                get_name_and_data(columns.ptr[i].via.map, column_name, data_obj, found_data, mask_obj, has_mask);

                if (!found_data || column_name.empty()) {
                    continue;
                }

                parse_chem_comp_bond_column(column_name, data_obj, mask_obj, has_mask, bond_data);
            }
            generate_bonds(bond_data, data.chem_comp_bonds, data.chem_comp_bonds_map);
        }

        namespace
        {
            struct StructConnData
            {
                struct LinkPartner
                {
                    std::vector<std::string> auth_asym_id;
                    std::vector<std::string> auth_comp_id;
                    std::vector<int32_t> auth_seq_id;
                    std::vector<std::string> label_atom_id;
                };
                std::vector<std::string> conn_type_id;
                LinkPartner ptnr1, ptnr2;
                std::vector<std::string> pdbx_value_order;

            };
            inline void generate_structconn(const StructConnData& struct_data, std::vector<BCIFFormat::BCIFData::StructConn>& out, BCIFFormat::BCIFData::StructConnMap& map)
            {
                // Build struct_conn entries
                size_t num_conns = struct_data.conn_type_id.size();
                out.reserve(num_conns);
                for (size_t i = 0; i < num_conns; ++i) {
                    out.push_back({});
                    BCIFData::StructConn& conn = out.back();
                    conn.conn_type_id = (i < struct_data.conn_type_id.size()) ? struct_data. conn_type_id[i] : "";
                    conn.ptnr1.label_asym_id = (i < struct_data.ptnr1.auth_asym_id.size()) ? struct_data. ptnr1.auth_asym_id[i] : "";
                    conn.ptnr1.label_comp_id = (i < struct_data.ptnr1.auth_comp_id.size()) ? struct_data.ptnr1.auth_comp_id[i] : "";
                    conn.ptnr1.label_seq_id = (i < struct_data.ptnr1.auth_seq_id.size()) ? struct_data.ptnr1.auth_seq_id[i] : -1;
                    conn.ptnr1.label_atom_id = (i < struct_data.ptnr1.label_atom_id.size()) ? struct_data.ptnr1.label_atom_id[i] : "";
                    conn.ptnr2.label_asym_id = (i < struct_data.ptnr2.auth_asym_id.size()) ? struct_data.ptnr2.auth_asym_id[i] : "";
                    conn.ptnr2.label_comp_id = (i < struct_data.ptnr2.auth_comp_id.size()) ? struct_data.ptnr2.auth_comp_id[i] : "";
                    conn.ptnr2.label_seq_id = (i < struct_data.ptnr2.auth_seq_id.size()) ? struct_data.ptnr2.auth_seq_id[i] : -1;
                    conn.ptnr2.label_atom_id = (i < struct_data.ptnr2.label_atom_id.size()) ? struct_data.ptnr2.label_atom_id[i] : "";
                    conn.pdbx_value_order = (i < struct_data.pdbx_value_order.size()) ? struct_data.pdbx_value_order[i] : "";
                    // WIP
                    BCIFFormat::BCIFData::StructConnMapKey k1{ conn.ptnr1.label_asym_id , conn.ptnr1.label_comp_id , conn.ptnr1.label_seq_id, conn.ptnr1.label_atom_id};
                    BCIFFormat::BCIFData::StructConnMapKey k2{ conn.ptnr2.label_asym_id , conn.ptnr2.label_comp_id , conn.ptnr2.label_seq_id, conn.ptnr2.label_atom_id};
                    map[k1] = &conn;
                    map[k2] = &conn;
                    // !WIP
                }
            }
            inline void parse_struct_conn_columns(const std::string& column_name, const msgpack::object& data_obj, const msgpack::object& mask_obj, const bool& has_mask, StructConnData& struct_data)
            {
                // Decode mask if present
                std::vector<int32_t> mask;
                if (has_mask) {
                    mask = decode_mask(mask_obj);
                }

                // Parse the columns we care about
                if (column_name == "conn_type_id") {
                    decode_column(data_obj, struct_data.conn_type_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.conn_type_id = apply_mask(struct_data.conn_type_id, mask);
                    }
                }
                else if (column_name == "ptnr1_auth_asym_id") {
                    decode_column(data_obj, struct_data.ptnr1.auth_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.ptnr1.auth_asym_id = apply_mask(struct_data.ptnr1.auth_asym_id, mask);
                    }
                }
                else if (column_name == "ptnr1_auth_comp_id") {
                    decode_column(data_obj, struct_data.ptnr1.auth_comp_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.ptnr1.auth_comp_id = apply_mask(struct_data.ptnr1.auth_comp_id, mask);
                    }
                }
                else if (column_name == "ptnr1_auth_seq_id") {
                    decode_column(data_obj, struct_data.ptnr1.auth_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.ptnr1.auth_seq_id = apply_mask(struct_data.ptnr1.auth_seq_id, mask);
                    }
                }
                else if (column_name == "ptnr1_label_atom_id") {
                    decode_column(data_obj, struct_data.ptnr1.label_atom_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.ptnr1.label_atom_id = apply_mask(struct_data.ptnr1.label_atom_id, mask);
                    }
                }
                else if (column_name == "ptnr2_auth_asym_id") {
                    decode_column(data_obj, struct_data.ptnr2.auth_asym_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.ptnr2.auth_asym_id = apply_mask(struct_data.ptnr2.auth_asym_id, mask);
                    }
                }
                else if (column_name == "ptnr2_auth_comp_id") {
                    decode_column(data_obj, struct_data.ptnr2.auth_comp_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.ptnr2.auth_comp_id = apply_mask(struct_data.ptnr2.auth_comp_id, mask);
                    }
                }
                else if (column_name == "ptnr2_auth_seq_id") {
                    decode_column(data_obj, struct_data.ptnr2.auth_seq_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.ptnr2.auth_seq_id = apply_mask(struct_data.ptnr2.auth_seq_id, mask);
                    }
                }
                else if (column_name == "ptnr2_label_atom_id") {
                    decode_column(data_obj, struct_data.ptnr2.label_atom_id);
                    if (has_mask && !mask.empty()) {
                        struct_data.ptnr2.label_atom_id = apply_mask(struct_data.ptnr2.label_atom_id, mask);
                    }
                }
                else if (column_name == "pdbx_value_order") {
                    decode_column(data_obj, struct_data.pdbx_value_order);
                    if (has_mask && !mask.empty()) {
                        struct_data.pdbx_value_order = apply_mask(struct_data.pdbx_value_order, mask);
                    }
                }
            }
        }
        void parse_struct_conn(const msgpack::object & category, BCIFData & data) {
            // Get columns array from category
            msgpack::object columns_obj;
            bool found_columns = false;

            auto cat_map = category.via.map;
            for (uint32_t i = 0; i < cat_map.size; ++i) {
                std::string key;
                cat_map.ptr[i].key.convert(key);
                if (key == "columns") {
                    columns_obj = cat_map.ptr[i].val;
                    found_columns = true;
                    break;
                }
            }

            if (!found_columns || columns_obj.type != msgpack::type::ARRAY) {
                return;
            }

            // Storage for the columns we need
            StructConnData struct_data;

            // Parse each column
            auto columns = columns_obj.via.array;
            for (uint32_t i = 0; i < columns.size; ++i) {
                if (columns.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                // Get column name and data
                std::string column_name;
                msgpack::object data_obj;
                msgpack::object mask_obj;
                bool found_data = false;
                bool has_mask = false;

                get_name_and_data(columns.ptr[i].via.map, column_name, data_obj, found_data, mask_obj, has_mask);

                if (!found_data || column_name.empty()) {
                    continue;
                }
                parse_struct_conn_columns(column_name, data_obj, mask_obj, has_mask, struct_data);
            }
            generate_structconn(struct_data, data.struct_conns, data.struct_conn_map);
        }

        namespace
        {
            inline void get_data_and_encoding(const msgpack::object_map& col_map, msgpack::object& data_obj, bool& found_data, msgpack::object& encoding_obj, bool& found_encoding)
            {

                for (uint32_t i = 0; i < col_map.size; ++i) {
                    std::string key;
                    col_map.ptr[i].key.convert(key);
                    if (key == "data") {
                        data_obj = col_map.ptr[i].val;
                        found_data = true;
                    }
                    else if (key == "encoding") {
                        encoding_obj = col_map.ptr[i].val;
                        found_encoding = true;
                    }
                }
            }
            inline std::string get_string_from_map(const msgpack::object_map& map, const char* key)
            {
                std::string ret;
                for (uint32_t j = 0; j < map.size; ++j) {
                    std::string current_key;
                    map.ptr[j].key.convert(current_key);
                    if (current_key == key) {
                        map.ptr[j].val.convert(ret);
                        break;
                    }
                }
                return ret;
            }
            inline void get_int_type_str(const int32_t& type_code, std::string& type_str)
            {
                if (type_code == 1) type_str = "Int8";
                else if (type_code == 2) type_str = "Int16";
                else if (type_code == 4) type_str = "Int32";
                else if (type_code == 5) type_str = "Uint8";
                else if (type_code == 6) type_str = "Uint16";
                else if (type_code == 8) type_str = "Uint32";
                else if (type_code == 32) type_str = "Float32";
                else if (type_code == 33) type_str = "Float64";
                else type_str = "Int32";  // fallback
            }

            inline void decode_integer_column_bytearray(
                const msgpack::object_map& enc_map,
                const msgpack::object_array& encodings,
                const msgpack::object& data_obj,
                std::vector<int32_t>& result,
                std::vector<uint8_t>& byte_data
            )
            {
                // Extract raw bytes from MessagePack binary
                const uint8_t* bytes = nullptr;
                size_t size = 0;

                if (data_obj.type == msgpack::type::BIN) {
                    bytes = reinterpret_cast<const uint8_t*>(data_obj.via.bin.ptr);
                    size = data_obj.via.bin.size;
                }
                else if (data_obj.type == msgpack::type::EXT) {
                    bytes = reinterpret_cast<const uint8_t*>(data_obj.via.ext.ptr);
                    size = data_obj.via.ext.size;
                }
                else {
                    throw format_error("the ByteArray data must be binary or extension type");
                }

                // Extract type parameter - can be string or integer
                int32_t type_code = 4;  // default to Int32
                std::string type_str = "";

                for (uint32_t j = 0; j < enc_map.size; ++j) {
                    std::string key;
                    enc_map.ptr[j].key.convert(key);
                    if (key == "type") {
                        auto& type_obj = enc_map.ptr[j].val;
                        if (type_obj.type == msgpack::type::POSITIVE_INTEGER) {
                            type_code = static_cast<int32_t>(type_obj.via.u64);
                        }
                        else if (type_obj.type == msgpack::type::STR) {
                            type_obj.convert(type_str);
                        }
                        break;
                    }
                }

                // Map type code to type string if needed
                if (type_str.empty()) {
                    get_int_type_str(type_code, type_str);
                }

                // Check if there are more encoding steps (like IntegerPacking)
                // If so, we should store raw bytes for the next step to process
                bool has_more_encodings = encodings.size > 1;

                // Decode based on type specification
                // If size is too small for the type or there are more encodings, store as bytes
                if (type_str == "Int8" && size >= 1 && !has_more_encodings) {
                    result.resize(size);
                    for (size_t k = 0; k < size; ++k) {
                        result[k] = static_cast<int32_t>(static_cast<int8_t>(bytes[k]));
                    }
                }
                else if (type_str == "Uint8" && size >= 1 && !has_more_encodings) {
                    result.resize(size);
                    for (size_t k = 0; k < size; ++k) {
                        result[k] = static_cast<int32_t>(bytes[k]);
                    }
                }
                else if (type_str == "Int16" && size >= 2 && !has_more_encodings) {
                    result.resize(size / 2);
                    for (size_t k = 0; k < result.size(); ++k) {
                        uint16_t val = bytes[k * 2] | (static_cast<uint16_t>(bytes[k * 2 + 1]) << 8);
                        result[k] = static_cast<int32_t>(static_cast<int16_t>(val));
                    }
                }
                else if (type_str == "Uint16" && size >= 2 && !has_more_encodings) {
                    result.resize(size / 2);
                    for (size_t k = 0; k < result.size(); ++k) {
                        uint16_t val = bytes[k * 2] | (static_cast<uint16_t>(bytes[k * 2 + 1]) << 8);
                        result[k] = static_cast<int32_t>(val);
                    }
                }
                else if (type_str == "Int32" && size >= 4 && !has_more_encodings) {
                    result.resize(size / 4);
                    for (size_t k = 0; k < result.size(); ++k) {
                        uint32_t val = bytes[k * 4]
                            | (static_cast<uint32_t>(bytes[k * 4 + 1]) << 8)
                            | (static_cast<uint32_t>(bytes[k * 4 + 2]) << 16)
                            | (static_cast<uint32_t>(bytes[k * 4 + 3]) << 24);
                        result[k] = static_cast<int32_t>(val);
                    }
                }
                else if (type_str == "Uint32" && size >= 4 && !has_more_encodings) {
                    result.resize(size / 4);
                    for (size_t k = 0; k < result.size(); ++k) {
                        uint32_t val = bytes[k * 4]
                            | (static_cast<uint32_t>(bytes[k * 4 + 1]) << 8)
                            | (static_cast<uint32_t>(bytes[k * 4 + 2]) << 16)
                            | (static_cast<uint32_t>(bytes[k * 4 + 3]) << 24);
                        result[k] = static_cast<int32_t>(val); // TODO : handling case where value > MAX_UINT32 / 2
                    }
                }
                else {
                    // Float32/Float64, or size too small, or more encodings to apply
                    // Store raw bytes for potential IntegerPacking or other encoding steps
                    byte_data.assign(bytes, bytes + size);
                }
            }

            inline void decode_integer_column_next_steps(
                const msgpack::object_map& enc_map,
                const msgpack::object_array& encodings,
                const msgpack::object& data_obj,
                const std::string& kind,
                std::vector<int32_t>& result,
                std::vector<uint8_t>& byte_data
            )
            {

                // Subsequent decoding steps
                if (kind == "IntegerPacking") {
                    // Extract byteCount and isUnsigned parameters
                    int32_t byte_count = 4;  // default
                    bool is_unsigned = false;  // default

                    for (uint32_t j = 0; j < enc_map.size; ++j) {
                        std::string key;
                        enc_map.ptr[j].key.convert(key);
                        if (key == "byteCount") {
                            enc_map.ptr[j].val.convert(byte_count);
                        }
                        else if (key == "isUnsigned") {
                            enc_map.ptr[j].val.convert(is_unsigned);
                        }
                    }
                    result = decode_integer_packing(byte_data, byte_count, is_unsigned);
                }
                else if (kind == "RunLength") {
                    // Extract srcSize parameter
                    int32_t src_size = 0;
                    for (uint32_t j = 0; j < enc_map.size; ++j) {
                        std::string key;
                        enc_map.ptr[j].key.convert(key);
                        if (key == "srcSize") {
                            enc_map.ptr[j].val.convert(src_size);
                            break;
                        }
                    }
                    result = decode_run_length(result, src_size);
                }
                else if (kind == "Delta") {
                    // Extract origin parameter
                    int32_t origin = 0;
                    for (uint32_t j = 0; j < enc_map.size; ++j) {
                        std::string key;
                        enc_map.ptr[j].key.convert(key);
                        if (key == "origin") {
                            enc_map.ptr[j].val.convert(origin);
                            break;
                        }
                    }
                    result = decode_delta(result, origin);
                }

                // Note: FixedPoint is NOT handled here - it's applied in decode_float_column
                // because it converts int32 to double
            }
        }
        void decode_column(const msgpack::object & column, std::vector<int32_t>& result) {
            // Column data structure: { data: <encoded>, encoding: [ {kind, ...}, ... ] }
            if (column.type != msgpack::type::MAP) {
                return ;
            }

            msgpack::object data_obj;
            msgpack::object encoding_obj;
            bool found_data = false;
            bool found_encoding = false;

            get_data_and_encoding(column.via.map, data_obj, found_data, encoding_obj, found_encoding);

            if (!found_data || !found_encoding) {
                return ;
            }

            if (encoding_obj.type != msgpack::type::ARRAY) {
                return ;
            }

            // Decode through the encoding chain (applied in reverse order)
            std::vector<uint8_t> byte_data;
            auto encodings = encoding_obj.via.array;

            // Start with the last encoding (innermost)
            for (uint32_t i = 0; i < encodings.size; ++i) {
                uint32_t idx = encodings.size - 1 - i;

                if (encodings.ptr[idx].type != msgpack::type::MAP) {
                    continue;
                }

                auto enc_map = encodings.ptr[idx].via.map;
                std::string kind = get_string_from_map(enc_map, "kind");

                if (i == 0) {
                    if (kind == "ByteArray") {
                        decode_integer_column_bytearray(enc_map, encodings, data_obj, result, byte_data);
                    }
                }
                else {
                    decode_integer_column_next_steps(enc_map, encodings, data_obj, kind, result, byte_data);
                }
            }
            return ;
        }

        namespace
        {
            inline void decode_float_column_float32(const uint8_t* bytes, const size_t& size, std::vector<double>& result)
            {
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
            inline void decode_float_column_float64(const uint8_t* bytes, const size_t& size, std::vector<double>& result)
            {
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
            inline void decode_float_column_raw_float(const msgpack::object& data_obj, const msgpack::object_array& encodings, std::vector<double>& result)
            {

                auto first_enc = encodings.ptr[encodings.size - 1].via.map;
                std::string kind;
                std::string type_str;
                int32_t type_code = 0;

                for (uint32_t j = 0; j < first_enc.size; ++j) {
                    std::string key;
                    first_enc.ptr[j].key.convert(key);
                    if (key == "kind") {
                        first_enc.ptr[j].val.convert(kind);
                    }
                    else if (key == "type") {
                        auto& type_obj = first_enc.ptr[j].val;
                        if (type_obj.type == msgpack::type::POSITIVE_INTEGER) {
                            type_code = static_cast<int32_t>(type_obj.via.u64);
                        }
                        else if (type_obj.type == msgpack::type::STR) {
                            type_obj.convert(type_str);
                        }
                    }
                }

                // Map type code to string: 32=Float32, 33=Float64
                if (type_code == 32) type_str = "Float32";
                else if (type_code == 33) type_str = "Float64";

                // Handle direct Float32/Float64 encoding
                if (kind == "ByteArray" && (type_str == "Float32" || type_str == "Float64")) {
                    if (data_obj.type != msgpack::type::BIN) {
                        throw format_error("the ByteArray data must be binary type");
                    }
                    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data_obj.via.bin.ptr);
                    size_t size = data_obj.via.bin.size;

                    if (type_str == "Float32") {
                        decode_float_column_float32(bytes, size, result);
                    }
                    else { // Float64
                        decode_float_column_float64(bytes, size, result);
                    }
                }
            }
        }
        void decode_column(const msgpack::object & column, std::vector<double>& result) {
            if (column.type != msgpack::type::MAP) {
                return ;
            }

            msgpack::object data_obj;
            msgpack::object encoding_obj;
            bool found_data = false;
            bool found_encoding = false;

            get_data_and_encoding(column.via.map, data_obj, found_data, encoding_obj, found_encoding);

            if (!found_data || !found_encoding || encoding_obj.type != msgpack::type::ARRAY) {
                return ;
            }

            msgpack::object_array encodings = encoding_obj.via.array;

            // Check if ByteArray has Float32/Float64 type (direct float encoding)
            if (encodings.size > 0 && encodings.ptr[encodings.size - 1].type == msgpack::type::MAP) {
                decode_float_column_raw_float(data_obj, encodings, result);
                return ;
            }

            // Otherwise, decode as integers first, then check for FixedPoint
            std::vector<int32_t> int_values; decode_column(column, int_values);

            // Look for FixedPoint encoding
            double factor = 1.0;
            std::string src_type = "Float64";  // default
            bool found_fixed_point = false;

            for (uint32_t i = 0; i < encodings.size; ++i) {
                if (encodings.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                std::string kind;
                auto enc_map = encodings.ptr[i].via.map;
                for (uint32_t j = 0; j < enc_map.size; ++j) {
                    std::string key;
                    enc_map.ptr[j].key.convert(key);
                    if (key == "kind") {
                        enc_map.ptr[j].val.convert(kind);
                    }
                    else if (key == "factor") {
                        enc_map.ptr[j].val.convert(factor);
                    }
                    else if (key == "srcType") {
                        enc_map.ptr[j].val.convert(src_type);
                    }
                }

                if (kind == "FixedPoint") {
                    found_fixed_point = true;
                    break;
                }
            }

            if (found_fixed_point) {
                // srcType is informational; we always decode to double
                result = decode_fixed_point(int_values, factor);
                return;
            }
            else {
                // No FixedPoint, convert integers to doubles
                result.resize(int_values.size());
                for (size_t i = 0; i < int_values.size(); ++i) {
                    result[i] = static_cast<double>(int_values[i]);
                }
                return ;
            }
        }

        void decode_column(const msgpack::object & column, std::vector<std::string>& result) {
            // Simple string array decoding. Might need some refinement in the future.
            if (column.type != msgpack::type::MAP) {
                return ;
            }

            msgpack::object data_obj;
            msgpack::object encoding_obj;
            bool found_data = false;
            bool found_encoding = false;

            get_data_and_encoding(column.via.map, data_obj, found_data, encoding_obj, found_encoding);

            if (!found_data || !found_encoding) {
                return ;
            }

            if (encoding_obj.type != msgpack::type::ARRAY) {
                return ;
            }

            // Look for StringArray encoding
            auto encodings = encoding_obj.via.array;
            for (uint32_t i = 0; i < encodings.size; ++i) {
                if (encodings.ptr[i].type != msgpack::type::MAP) {
                    continue;
                }

                std::string kind;
                auto enc_map = encodings.ptr[i].via.map;
                for (uint32_t j = 0; j < enc_map.size; ++j) {
                    std::string key;
                    enc_map.ptr[j].key.convert(key);
                    if (key == "kind") {
                        enc_map.ptr[j].val.convert(kind);
                        break;
                    }
                }

                if (kind == "StringArray") {
                    result = decode_string_array(encodings.ptr[i], data_obj);
                    return;
                }
            }

            return ;
        }

        std::vector<int32_t> decode_byte_array(const msgpack::object & data) {
            // ByteArray decoder: converts byte array from MessagePack to int32 array
            if (data.type != msgpack::type::BIN) {
                throw format_error("the ByteArray data must be binary type");
            }

            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.via.bin.ptr);
            size_t size = data.via.bin.size;

            std::vector<int32_t> result(size);
            for (size_t i = 0; i < size; ++i) {
                result[i] = static_cast<int32_t>(bytes[i]);
            }

            return result;
        }
        namespace
        {
            inline void decode_integer_packing_1byte(const std::vector<uint8_t>& data,
                const int32_t& byte_count, const bool& is_unsigned, std::vector<int32_t>& result)
            {
                // Determine the upper and lower limits for 8-bit values
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

                    // Accumulate consecutive limit values
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

                    // Always add the final non-limit value (or 0 if ended on limit)
                    if (i < data.size()) {
                        value += t;
                        i++;
                    }
                    result.push_back(value);
                }
            }
            inline void decode_integer_packing_2bytes(const std::vector<uint8_t>& data,
                const int32_t& byte_count, const bool& is_unsigned, std::vector<int32_t>& result)
            {
                // Determine the upper and lower limits for 16-bit values
                const int32_t upper_limit = is_unsigned ? UINT16_MAX : INT16_MAX;
                const int32_t lower_limit = is_unsigned ? 0 : INT16_MIN;

                size_t i = 0;
                while (i + 1 < data.size()) {
                    int32_t value = 0;

                    // Read first 16-bit value
                    uint16_t raw_value = data[i] | (static_cast<uint16_t>(data[i + 1]) << 8);
                    int32_t t;
                    if (is_unsigned) {
                        t = static_cast<int32_t>(raw_value);
                    }
                    else {
                        t = static_cast<int32_t>(static_cast<int16_t>(raw_value));
                    }

                    // Accumulate consecutive limit values
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

                    // Always add the final non-limit value (or 0 if ended on limit)
                    if (i + 1 < data.size()) {
                        value += t;
                        i += 2;
                    }
                    result.push_back(value);
                }
            }
        }
        std::vector<int32_t> decode_integer_packing(const std::vector<uint8_t>&data,
            int32_t byte_count, bool is_unsigned) {
            // IntegerPacking decoder: unpacks integers from byte array
            // When values exceed the bit-width limit, they're encoded as multiple consecutive
            // max values that need to be summed: e.g. 128 in signed 8-bit becomes [127, 1]
            std::vector<int32_t> result;

            if (byte_count == 1) {
                decode_integer_packing_1byte(data, byte_count, is_unsigned, result);
            }
            else if (byte_count == 2) {
                decode_integer_packing_2bytes(data, byte_count, is_unsigned, result);
                
            }
            else if (byte_count == 4) {
                // 32-bit values don't need overflow handling
                result.resize(data.size() / 4);
                for (size_t i = 0; i < result.size(); ++i) {
                    uint32_t value = data[i * 4]
                        | (static_cast<uint32_t>(data[i * 4 + 1]) << 8)
                        | (static_cast<uint32_t>(data[i * 4 + 2]) << 16)
                        | (static_cast<uint32_t>(data[i * 4 + 3]) << 24);
                    result[i] = static_cast<int32_t>(value);
                }
            }
            else {
                throw format_error("the IntegerPacking: unsupported byte count {}", byte_count);
            }

            return result;
        }

        std::vector<int32_t> decode_delta(const std::vector<int32_t>&data, int32_t origin) {
            // Delta decoder: reconstructs original values from delta-encoded data
            if (data.empty()) {
                return {};
            }

            std::vector<int32_t> result(data.size());
            result[0] = data[0] + origin;

            for (size_t i = 1; i < data.size(); ++i) {
                result[i] = result[i - 1] + data[i];
            }

            return result;
        }

        std::vector<double> decode_fixed_point(const std::vector<int32_t>&data, double factor) {
            // FixedPoint decoder: converts fixed-point integers to floating-point values
            std::vector<double> result(data.size());

            for (size_t i = 0; i < data.size(); ++i) {
                result[i] = static_cast<double>(data[i]) / factor;
            }

            return result;
        }

        std::vector<int32_t> decode_run_length(const std::vector<int32_t>& data, int32_t src_size) {
            // RunLength decoder: expands run-length encoded data
            // Data format: [value1, count1, value2, count2, ...]
            // Output: [value1, value1, ...(count1 times), value2, value2, ...(count2 times), ...]
            std::vector<int32_t> result;
            result.reserve(src_size);

            for (size_t i = 0; i + 1 < data.size(); i += 2) {
                int32_t value = data[i];
                int32_t count = data[i + 1];
                for (int32_t j = 0; j < count; ++j) {
                    result.push_back(value);
                }
            }

            return result;
        }

        std::vector<int32_t> decode_mask(const msgpack::object & mask_obj) {
            // Mask is encoded just like a regular integer column
            // Mask values: 0=present, 1="." (not present), 2="?" (unknown)
            std::vector<int32_t> result;
            decode_column(mask_obj, result);
            return result;
        }

        namespace
        {
            inline void decode_string_array_with_indices(const msgpack::object& data_encoding_obj, const msgpack::object& data, std::vector<int32_t>& indices)
            {
                // Apply dataEncoding chain in reverse order
                auto encodings = data_encoding_obj.via.array;
                std::vector<int32_t> result;

                // Start with the last encoding (ByteArray)
                for (int idx = encodings.size - 1; idx >= 0; --idx) {
                    auto& enc = encodings.ptr[idx];
                    if (enc.type != msgpack::type::MAP) continue;

                    std::string kind;
                    auto enc_map = enc.via.map;
                    for (uint32_t j = 0; j < enc_map.size; ++j) {
                        std::string key;
                        enc_map.ptr[j].key.convert(key);
                        if (key == "kind") {
                            enc_map.ptr[j].val.convert(kind);
                            break;
                        }
                    }

                    if (idx == static_cast<int>(encodings.size) - 1) {
                        // First step: decode ByteArray from raw data
                        if (kind == "ByteArray") {
                            result = decode_byte_array(data);
                        }
                    }
                    else {
                        // Subsequent steps: apply encoding to result
                        if (kind == "IntegerPacking") {
                            // Extract parameters
                            int32_t byte_count = 4;
                            bool is_unsigned = false;
                            for (uint32_t j = 0; j < enc_map.size; ++j) {
                                std::string key;
                                enc_map.ptr[j].key.convert(key);
                                if (key == "byteCount") {
                                    enc_map.ptr[j].val.convert(byte_count);
                                }
                                else if (key == "isUnsigned") {
                                    enc_map.ptr[j].val.convert(is_unsigned);
                                }
                            }
                            // Convert int32 to uint8_t for decode_integer_packing
                            std::vector<uint8_t> byte_data(result.size());
                            for (size_t i = 0; i < result.size(); ++i) {
                                byte_data[i] = static_cast<uint8_t>(result[i]);
                            }
                            result = decode_integer_packing(byte_data, byte_count, is_unsigned);
                        }
                        else if (kind == "RunLength") {
                            // Extract srcSize parameter
                            int32_t src_size = 0;
                            for (uint32_t j = 0; j < enc_map.size; ++j) {
                                std::string key;
                                enc_map.ptr[j].key.convert(key);
                                if (key == "srcSize") {
                                    enc_map.ptr[j].val.convert(src_size);
                                    break;
                                }
                            }
                            result = decode_run_length(result, src_size);
                        }
                        else if (kind == "Delta") {
                            // Extract origin parameter
                            int32_t origin = 0;
                            for (uint32_t j = 0; j < enc_map.size; ++j) {
                                std::string key;
                                enc_map.ptr[j].key.convert(key);
                                if (key == "origin") {
                                    enc_map.ptr[j].val.convert(origin);
                                    break;
                                }
                            }
                            result = decode_delta(result, origin);
                        }
                    }
                }
                indices = result;
            }
            inline void decode_string_array_with_offset(const msgpack::object& offsets_obj,const msgpack::object& offset_encoding_obj, const msgpack::object& data, std::vector<int32_t>& offsets)
            {
                auto encodings = offset_encoding_obj.via.array;
                std::vector<int32_t> result;

                // Start with the last encoding (ByteArray)
                for (int idx = encodings.size - 1; idx >= 0; --idx) {
                    auto& enc = encodings.ptr[idx];
                    if (enc.type != msgpack::type::MAP) continue;

                    std::string kind;
                    auto enc_map = enc.via.map;
                    for (uint32_t j = 0; j < enc_map.size; ++j) {
                        std::string key;
                        enc_map.ptr[j].key.convert(key);
                        if (key == "kind") {
                            enc_map.ptr[j].val.convert(kind);
                            break;
                        }
                    }

                    if (idx == static_cast<int>(encodings.size) - 1) {
                        // First step: decode ByteArray from offsets data
                        if (kind == "ByteArray") {
                            result = decode_byte_array(offsets_obj);
                        }
                    }
                    else {
                        // Subsequent steps: apply encoding
                        if (kind == "IntegerPacking") {
                            // Extract parameters
                            int32_t byte_count = 4;
                            bool is_unsigned = false;
                            for (uint32_t j = 0; j < enc_map.size; ++j) {
                                std::string key;
                                enc_map.ptr[j].key.convert(key);
                                if (key == "byteCount") {
                                    enc_map.ptr[j].val.convert(byte_count);
                                }
                                else if (key == "isUnsigned") {
                                    enc_map.ptr[j].val.convert(is_unsigned);
                                }
                            }
                            // Convert int32 to uint8_t for decode_integer_packing
                            std::vector<uint8_t> byte_data(result.size());
                            for (size_t i = 0; i < result.size(); ++i) {
                                byte_data[i] = static_cast<uint8_t>(result[i]);
                            }
                            result = decode_integer_packing(byte_data, byte_count, is_unsigned);
                        }
                        else if (kind == "RunLength") {
                            int32_t src_size = 0;
                            for (uint32_t j = 0; j < enc_map.size; ++j) {
                                std::string key;
                                enc_map.ptr[j].key.convert(key);
                                if (key == "srcSize") {
                                    enc_map.ptr[j].val.convert(src_size);
                                }
                            }
                            result = decode_run_length(result, src_size);
                        }
                        else if (kind == "Delta") {
                            int32_t origin = 0;
                            for (uint32_t j = 0; j < enc_map.size; ++j) {
                                std::string key;
                                enc_map.ptr[j].key.convert(key);
                                if (key == "origin") {
                                    enc_map.ptr[j].val.convert(origin);
                                }
                            }
                            result = decode_delta(result, origin);
                        }
                    }
                }
                offsets = result;
            }
        }
        std::vector<std::string> decode_string_array(const msgpack::object & encoding, const msgpack::object & data) {
            // StringArray decoder: decodes string arrays
            if (encoding.type != msgpack::type::MAP) {
                return {};
            }

            // Get dataEncoding, stringData, offsetEncoding, and offsets from encoding spec
            msgpack::object data_encoding_obj;
            msgpack::object string_data_obj;
            msgpack::object offset_encoding_obj;
            msgpack::object offsets_obj;

            auto enc_map = encoding.via.map;
            for (uint32_t i = 0; i < enc_map.size; ++i) {
                std::string key;
                enc_map.ptr[i].key.convert(key);
                if (key == "dataEncoding") {
                    data_encoding_obj = enc_map.ptr[i].val;
                }
                else if (key == "stringData") {
                    string_data_obj = enc_map.ptr[i].val;
                }
                else if (key == "offsetEncoding") {
                    offset_encoding_obj = enc_map.ptr[i].val;
                }
                else if (key == "offsets") {
                    offsets_obj = enc_map.ptr[i].val;
                }
            }

            // Decode string data
            std::string string_data;
            if (string_data_obj.type == msgpack::type::STR) {
                string_data_obj.convert(string_data);
            }
            else {
                return {};
            }


            // Decode the indices from the data using dataEncoding chain
            std::vector<int32_t> indices;
            if (data_encoding_obj.type == msgpack::type::ARRAY) {
                decode_string_array_with_indices(data_encoding_obj, data, indices);
            }

            // Decode the offsets using offsetEncoding chain
            std::vector<int32_t> offsets;
            if (offset_encoding_obj.type == msgpack::type::ARRAY) {
                decode_string_array_with_offset(offsets_obj, offset_encoding_obj, data, offsets);
            }


            if (offsets.empty() || indices.empty()) {
                return {};
            }

            // Map indices to strings using offsets
            std::vector<std::string> result;
            result.reserve(indices.size());

            for (size_t i = 0; i < indices.size(); ++i) {
                int32_t idx = indices[i];
                if (idx < 0 || idx >= static_cast<int32_t>(offsets.size()) - 1) {
                    result.push_back("");
                    continue;
                }

                size_t start = static_cast<size_t>(offsets[idx]);
                size_t end = static_cast<size_t>(offsets[idx + 1]);

                if (start <= end && end <= string_data.size()) {
                    result.push_back(string_data.substr(start, end - start));
                }
                else {
                    result.push_back("");
                }
            }

            return result;
        }

        // =============================================================================
        // BCIF Encoder Implementations (for writing)
        // =============================================================================

        std::vector<uint8_t> encode_byte_array_float64(const std::vector<double>& data) {
            // Encode Float64 array to little-endian byte array
            std::vector<uint8_t> result(data.size() * 8);

            for (size_t i = 0; i < data.size(); ++i) {
                uint64_t bits;
                std::memcpy(&bits, &data[i], sizeof(double));

                // Store as little-endian
                result[i * 8 + 0] = static_cast<uint8_t>(bits & 0xFF);
                result[i * 8 + 1] = static_cast<uint8_t>((bits >> 8) & 0xFF);
                result[i * 8 + 2] = static_cast<uint8_t>((bits >> 16) & 0xFF);
                result[i * 8 + 3] = static_cast<uint8_t>((bits >> 24) & 0xFF);
                result[i * 8 + 4] = static_cast<uint8_t>((bits >> 32) & 0xFF);
                result[i * 8 + 5] = static_cast<uint8_t>((bits >> 40) & 0xFF);
                result[i * 8 + 6] = static_cast<uint8_t>((bits >> 48) & 0xFF);
                result[i * 8 + 7] = static_cast<uint8_t>((bits >> 56) & 0xFF);
            }

            return result;
        }

        std::vector<uint8_t> encode_byte_array_int32(const std::vector<int32_t>& data) {
            // Encode Int32 array to little-endian byte array
            std::vector<uint8_t> result(data.size() * 4);

            for (size_t i = 0; i < data.size(); ++i) {
                uint32_t value = static_cast<uint32_t>(data[i]);

                // Store as little-endian
                result[i * 4 + 0] = static_cast<uint8_t>(value & 0xFF);
                result[i * 4 + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                result[i * 4 + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                result[i * 4 + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
            }

            return result;
        }

        std::vector<int32_t> encode_delta(const std::vector<int32_t>& data, int32_t& origin_out) {
            // Delta encoder: stores differences between consecutive values
            if (data.empty()) {
                origin_out = 0;
                return {};
            }

            origin_out = data[0];
            std::vector<int32_t> result;
            result.reserve(data.size());

            for (size_t i = 0; i < data.size(); ++i) {
                if (i == 0) {
                    result.push_back(0);  // First value is always 0
                } else {
                    result.push_back(data[i] - data[i - 1]);
                }
            }

            return result;
        }

        std::vector<int32_t> encode_run_length(const std::vector<int32_t>& data) {
            // RunLength encoder: compresses repeated values
            // Output format: [value1, count1, value2, count2, ...]
            if (data.empty()) {
                return {};
            }

            std::vector<int32_t> result;
            int32_t current_value = data[0];
            int32_t count = 1;

            for (size_t i = 1; i < data.size(); ++i) {
                if (data[i] == current_value) {
                    count++;
                } else {
                    result.push_back(current_value);
                    result.push_back(count);
                    current_value = data[i];
                    count = 1;
                }
            }

            // Push last run
            result.push_back(current_value);
            result.push_back(count);

            return result;
        }

        std::vector<uint8_t> encode_integer_packing(const std::vector<int32_t>& data,
            int32_t& byte_count_out, bool& is_unsigned_out) {
            // IntegerPacking encoder: packs integers into smallest byte representation
            if (data.empty()) {
                byte_count_out = 1;
                is_unsigned_out = true;
                return {};
            }

            // Determine min/max to choose byte count
            int32_t min_val = data[0];
            int32_t max_val = data[0];
            for (const auto& val : data) {
                if (val < min_val) min_val = val;
                if (val > max_val) max_val = val;
            }

            // Determine if unsigned and byte count
            is_unsigned_out = (min_val >= 0);

            if (is_unsigned_out) {
                if (max_val <= 255) {
                    byte_count_out = 1;
                } else if (max_val <= 65535) {
                    byte_count_out = 2;
                } else {
                    byte_count_out = 4;
                }
            } else {
                if (min_val >= -128 && max_val <= 127) {
                    byte_count_out = 1;
                } else if (min_val >= -32768 && max_val <= 32767) {
                    byte_count_out = 2;
                } else {
                    byte_count_out = 4;
                }
            }

            // Pack the data
            std::vector<uint8_t> result(data.size() * byte_count_out);

            for (size_t i = 0; i < data.size(); ++i) {
                uint32_t value = static_cast<uint32_t>(data[i]);

                for (int32_t b = 0; b < byte_count_out; ++b) {
                    result[i * byte_count_out + b] = static_cast<uint8_t>((value >> (b * 8)) & 0xFF);
                }
            }

            return result;
        }

        std::pair<std::vector<int32_t>, std::vector<uint8_t>> encode_string_array(
            const std::vector<std::string>& strings) {
            // StringArray encoder: creates string data and indices
            // Returns: (indices, offsets_binary)

            if (strings.empty()) {
                return {{}, {}};
            }

            // Build unique strings and create indices
            std::vector<std::string> unique_strings;
            std::vector<int32_t> indices;
            std::map<std::string, int32_t> string_to_index;

            for (const auto& str : strings) {
                auto it = string_to_index.find(str);
                if (it == string_to_index.end()) {
                    int32_t idx = static_cast<int32_t>(unique_strings.size());
                    unique_strings.push_back(str);
                    string_to_index[str] = idx;
                    indices.push_back(idx);
                } else {
                    indices.push_back(it->second);
                }
            }

            // Create offsets array
            std::vector<int32_t> offsets;
            offsets.push_back(0);
            int32_t current_offset = 0;
            for (const auto& str : unique_strings) {
                current_offset += static_cast<int32_t>(str.size());
                offsets.push_back(current_offset);
            }

            // Encode offsets to binary
            std::vector<uint8_t> offsets_binary = encode_byte_array_int32(offsets);

            return {indices, offsets_binary};
        }

        // =============================================================================
        // Helper functions for building MessagePack structure (for writing)
        // =============================================================================

        namespace
        {
            struct AtomSiteCatogryEncodingData
            {
                AtomSiteCatogryEncodingData(const size_t& natoms)
                {
                    atom_x.reserve(natoms);
                    atom_y.reserve(natoms);
                    atom_z.reserve(natoms);
                    type_symbols.reserve(natoms);
                    atom_names.reserve(natoms);
                    atom_ids.reserve(natoms);
                    residue_names.reserve(natoms);
                    chain_ids.reserve(natoms);
                    residue_ids.reserve(natoms);
                    auth_chain_ids.reserve(natoms);
                    auth_residue_ids.reserve(natoms);
                    insertion_codes.reserve(natoms);
                }

                std::vector<double> atom_x, atom_y, atom_z;
                std::vector<std::string> type_symbols;
                std::vector<std::string> atom_names;          // label_atom_id
                std::vector<int32_t> atom_ids;                // id
                std::vector<std::string> residue_names;
                std::vector<std::string> chain_ids;           // label_asym_id
                std::vector<int32_t> residue_ids;             // label_seq_id
                std::vector<std::string> auth_chain_ids;      // auth_asym_id
                std::vector<int32_t> auth_residue_ids;        // auth_seq_id
                std::vector<std::string> insertion_codes;


            };

            inline void pack(msgpack::packer<msgpack::sbuffer>& pk, const AtomSiteCatogryEncodingData& atom_site_data)
            {

                // Category map
                pk.pack_map(2);  // name + columns

                pk.pack("name");
                pk.pack("_atom_site");

                pk.pack("columns");
                pk.pack_array(12);  // id, Cartn_x, Cartn_y, Cartn_z, type_symbol, label_atom_id, label_comp_id, label_asym_id, label_seq_id, auth_asym_id, auth_seq_id, pdbx_PDB_ins_code

                // === id column (atom serial number) ===
                encode_integer_column(pk, "id", atom_site_data.atom_ids);

                // === Cartn_x column ===
                encode_float_column(pk, "Cartn_x", atom_site_data.atom_x);

                // === Cartn_y column ===
                encode_float_column(pk, "Cartn_y", atom_site_data.atom_y);

                // === Cartn_z column ===
                encode_float_column(pk, "Cartn_z", atom_site_data.atom_z);

                // === type_symbol column ===
                encode_string_column(pk, "type_symbol", atom_site_data.type_symbols);

                // === label_atom_id column (atom name) ===
                encode_string_column(pk, "label_atom_id", atom_site_data.atom_names);

                // === label_comp_id column (residue name) ===
                encode_string_column(pk, "label_comp_id", atom_site_data.residue_names);

                // === label_asym_id column (label chain ID - primary for residue identification) ===
                encode_string_column(pk, "label_asym_id", atom_site_data.chain_ids);

                // === label_seq_id column (label residue ID - primary for residue identification) ===
                encode_integer_column(pk, "label_seq_id", atom_site_data.residue_ids);

                // === auth_asym_id column (author's chain ID - for round-trip) ===
                encode_string_column(pk, "auth_asym_id", atom_site_data.auth_chain_ids);

                // === auth_seq_id column (author's residue ID - for round-trip) ===
                encode_integer_column(pk, "auth_seq_id", atom_site_data.auth_residue_ids);

                // === pdbx_PDB_ins_code column (insertion code) ===
                encode_string_column(pk, "pdbx_PDB_ins_code", atom_site_data.insertion_codes);
            }
            inline void fill(const Frame& frame, AtomSiteCatogryEncodingData& atom_site_data)
            {
                size_t natoms = frame.size();
                auto positions = frame.positions();
                const auto& topology = frame.topology();
                const auto& residues = topology.residues();
                std::vector<size_t> atom_to_residue(natoms, SIZE_MAX);
                for (size_t res_idx = 0; res_idx < residues.size(); ++res_idx) {
                    for (size_t atom_idx : residues[res_idx]) {
                        if (atom_idx < natoms) {
                            atom_to_residue[atom_idx] = res_idx;
                        }
                    }
                }

                for (size_t i = 0; i < natoms; ++i) {
                    atom_site_data.atom_x.push_back(positions[i][0]);
                    atom_site_data.atom_y.push_back(positions[i][1]);
                    atom_site_data.atom_z.push_back(positions[i][2]);
                    atom_site_data.type_symbols.push_back(frame[i].type());
                    atom_site_data.atom_names.push_back(frame[i].name());

                    // Get atom ID from property, or use index+1 as default
                    auto id_prop = frame[i].get("id");
                    int32_t atom_id = id_prop ? static_cast<int32_t>(id_prop->as_double()) : static_cast<int32_t>(i + 1);
                    atom_site_data.atom_ids.push_back(atom_id);

                    // Extract residue information if atom belongs to a residue
                    if (atom_to_residue[i] != SIZE_MAX) {
                        const auto& residue = residues[atom_to_residue[i]];
                        atom_site_data.residue_names.push_back(residue.name());

                        // Get label chain ID from residue property (primary)
                        auto chain_prop = residue.get("chainid");
                        std::string label_chain = chain_prop ? chain_prop->as_string() : "A";
                        atom_site_data.chain_ids.push_back(label_chain);

                        // Get label residue ID (primary)
                        int32_t label_resid = residue.id() ? static_cast<int32_t>(*residue.id()) : 1;
                        atom_site_data.residue_ids.push_back(label_resid);

                        // Get auth chain ID (for round-trip, falls back to label if not set)
                        auto auth_chain_prop = residue.get("chainname");
                        atom_site_data.auth_chain_ids.push_back(auth_chain_prop ? auth_chain_prop->as_string() : label_chain);

                        // Get auth residue ID (for round-trip, falls back to label if not set)
                        auto auth_resid_prop = residue.get("auth_seq_id");
                        atom_site_data.auth_residue_ids.push_back(auth_resid_prop ? static_cast<int32_t>(auth_resid_prop->as_double()) : label_resid);

                        // Get insertion code from residue property
                        auto ins_prop = residue.get("insertion_code");
                        atom_site_data.insertion_codes.push_back(ins_prop ? ins_prop->as_string() : "?");
                    }
                    else {
                        // Atom not in any residue - use defaults
                        atom_site_data.residue_names.push_back("UNK");
                        atom_site_data.chain_ids.push_back("A");
                        atom_site_data.residue_ids.push_back(1);
                        atom_site_data.auth_chain_ids.push_back("A");
                        atom_site_data.auth_residue_ids.push_back(1);
                        atom_site_data.insertion_codes.push_back("?");
                    }
                }
            }
        }
        void encode_atom_site_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame) 
        {
            AtomSiteCatogryEncodingData atom_site_data(frame.size());

            fill(frame, atom_site_data);
            pack(pk, atom_site_data);
        }
        void encode_cell_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame) {
            const auto& cell = frame.cell();
            auto lengths = cell.lengths();
            auto angles = cell.angles();

            // Category map
            pk.pack_map(2);  // name + columns

            pk.pack("name");
            pk.pack("_cell");

            pk.pack("columns");
            pk.pack_array(6);  // length_a, length_b, length_c, angle_alpha, angle_beta, angle_gamma

            // Cell parameters - encode as direct values (simple approach for Phase 2)
            encode_float_column(pk, "length_a", {lengths[0]});
            encode_float_column(pk, "length_b", {lengths[1]});
            encode_float_column(pk, "length_c", {lengths[2]});
            encode_float_column(pk, "angle_alpha", {angles[0]});
            encode_float_column(pk, "angle_beta", {angles[1]});
            encode_float_column(pk, "angle_gamma", {angles[2]});
        }

        namespace
        {
            struct SSRange {
                std::string chain_id;
                int32_t beg_seq_id;
                int32_t end_seq_id;
                std::string conf_type_id;
                int32_t helix_class;  // 0 if not a helix, 1-10 for helix types
            };
            inline void encode_struct_conf_category_fill_ranges(const Frame& frame, std::vector<SSRange>& ss_ranges)
            {
                const auto& topology = frame.topology();
                const auto& residues = topology.residues();
                // Build secondary structure ranges from residues
                std::string current_ss_type;
                std::string current_chain;
                int32_t range_start = -1;
                int32_t range_end = -1;

                for (const auto& residue : residues) {
                    auto ss_prop = residue.get("secondary_structure");
                    auto chain_prop = residue.get("chainid");

                    if (!residue.id() || !chain_prop) {
                        continue;
                    }

                    std::string chain_id = chain_prop->as_string();
                    int32_t seq_id = static_cast<int32_t>(*residue.id());
                    std::string ss_type = ss_prop ? ss_prop->as_string() : "";

                    if (ss_type.empty()) {
                        // No secondary structure - end current range if any
                        if (range_start >= 0) {
                            // Convert PDB-style name to BCIF conf_type_id and get helix class
                            std::string bcif_type = pdb_to_bcif_secondary_structure(current_ss_type);
                            int32_t helix_class = get_helix_class_from_pdb_ss(current_ss_type);
                            ss_ranges.push_back({ current_chain, range_start, range_end, bcif_type, helix_class });
                            range_start = -1;
                        }
                    }
                    else if (ss_type == current_ss_type && chain_id == current_chain && seq_id == range_end + 1) {
                        // Continue current range
                        range_end = seq_id;
                    }
                    else {
                        // Start new range (after ending current if any)
                        if (range_start >= 0) {
                            // Convert PDB-style name to BCIF conf_type_id and get helix class
                            std::string bcif_type = pdb_to_bcif_secondary_structure(current_ss_type);
                            int32_t helix_class = get_helix_class_from_pdb_ss(current_ss_type);
                            ss_ranges.push_back({ current_chain, range_start, range_end, bcif_type, helix_class });
                        }
                        current_ss_type = ss_type;
                        current_chain = chain_id;
                        range_start = seq_id;
                        range_end = seq_id;
                    }
                }

                // Add final range if any
                if (range_start >= 0) {
                    // Convert PDB-style name to BCIF conf_type_id and get helix class
                    std::string bcif_type = pdb_to_bcif_secondary_structure(current_ss_type);
                    int32_t helix_class = get_helix_class_from_pdb_ss(current_ss_type);
                    ss_ranges.push_back({ current_chain, range_start, range_end, bcif_type, helix_class });
                }
            }

        }
        void encode_struct_conf_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame) {

            // Structure to hold secondary structure ranges
            std::vector<SSRange> ss_ranges;
            encode_struct_conf_category_fill_ranges(frame, ss_ranges);
            

            // If no secondary structures, write empty category
            if (ss_ranges.empty()) {
                pk.pack_map(2);  // name + columns
                pk.pack("name");
                pk.pack("_struct_conf");
                pk.pack("columns");
                pk.pack_array(0);
                return;
            }

            // Extract data for columns
            std::vector<std::string> conf_type_ids;
            std::vector<int32_t> pdbx_PDB_helix_classes;
            std::vector<std::string> beg_label_asym_ids;
            std::vector<int32_t> beg_label_seq_ids;
            std::vector<std::string> end_label_asym_ids;
            std::vector<int32_t> end_label_seq_ids;

            for (const auto& range : ss_ranges) {
                conf_type_ids.push_back(range.conf_type_id);
                pdbx_PDB_helix_classes.push_back(range.helix_class);
                beg_label_asym_ids.push_back(range.chain_id);
                beg_label_seq_ids.push_back(range.beg_seq_id);
                end_label_asym_ids.push_back(range.chain_id);
                end_label_seq_ids.push_back(range.end_seq_id);
            }

            // Category map
            pk.pack_map(2);  // name + columns

            pk.pack("name");
            pk.pack("_struct_conf");

            pk.pack("columns");
            pk.pack_array(6);  // conf_type_id, pdbx_PDB_helix_class, beg_label_asym_id, beg_label_seq_id, end_label_asym_id, end_label_seq_id

            encode_string_column(pk, "conf_type_id", conf_type_ids);
            encode_integer_column(pk, "pdbx_PDB_helix_class", pdbx_PDB_helix_classes);
            encode_string_column(pk, "beg_label_asym_id", beg_label_asym_ids);
            encode_integer_column(pk, "beg_label_seq_id", beg_label_seq_ids);
            encode_string_column(pk, "end_label_asym_id", end_label_asym_ids);
            encode_integer_column(pk, "end_label_seq_id", end_label_seq_ids);
        }

        namespace
        {
            struct SheetRange {
                std::string chain_id;
                int32_t beg_seq_id;
                int32_t end_seq_id;
            };
            inline void encode_struct_sheet_range_category_fill_ranges(const Frame& frame, std::vector<SheetRange>& sheet_ranges)
            {

                const auto& topology = frame.topology();
                const auto& residues = topology.residues();
                // Build beta sheet ranges from residues
                std::string current_chain;
                int32_t range_start = -1;
                int32_t range_end = -1;

                for (const auto& residue : residues) {
                    auto ss_prop = residue.get("secondary_structure");
                    auto chain_prop = residue.get("chainid");

                    if (!residue.id() || !chain_prop) {
                        continue;
                    }

                    std::string chain_id = chain_prop->as_string();
                    int32_t seq_id = static_cast<int32_t>(*residue.id());
                    std::string ss_type = ss_prop ? ss_prop->as_string() : "";

                    // Check if this is a beta sheet residue
                    bool is_sheet = (ss_type == "extended");

                    if (!is_sheet) {
                        // Not a sheet - end current range if any
                        if (range_start >= 0) {
                            sheet_ranges.push_back({ current_chain, range_start, range_end });
                            range_start = -1;
                        }
                    }
                    else if (chain_id == current_chain && seq_id == range_end + 1) {
                        // Continue current sheet range
                        range_end = seq_id;
                    }
                    else {
                        // Start new sheet range (after ending current if any)
                        if (range_start >= 0) {
                            sheet_ranges.push_back({ current_chain, range_start, range_end });
                        }
                        current_chain = chain_id;
                        range_start = seq_id;
                        range_end = seq_id;
                    }
                }

                // Add final range if any
                if (range_start >= 0) {
                    sheet_ranges.push_back({ current_chain, range_start, range_end });
                }
            }
        }
        void encode_struct_sheet_range_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame) {

            std::vector<SheetRange> sheet_ranges;
            encode_struct_sheet_range_category_fill_ranges(frame, sheet_ranges);

            // If no beta sheets, write empty category
            if (sheet_ranges.empty()) {
                pk.pack_map(2);  // name + columns
                pk.pack("name");
                pk.pack("_struct_sheet_range");
                pk.pack("columns");
                pk.pack_array(0);
                return;
            }

            // Extract data for columns
            std::vector<std::string> beg_label_asym_ids;
            std::vector<int32_t> beg_label_seq_ids;
            std::vector<std::string> end_label_asym_ids;
            std::vector<int32_t> end_label_seq_ids;

            for (const auto& range : sheet_ranges) {
                beg_label_asym_ids.push_back(range.chain_id);
                beg_label_seq_ids.push_back(range.beg_seq_id);
                end_label_asym_ids.push_back(range.chain_id);
                end_label_seq_ids.push_back(range.end_seq_id);
            }

            // Category map
            pk.pack_map(2);  // name + columns

            pk.pack("name");
            pk.pack("_struct_sheet_range");

            pk.pack("columns");
            pk.pack_array(4);  // beg_label_asym_id, beg_label_seq_id, end_label_asym_id, end_label_seq_id

            encode_string_column(pk, "beg_label_asym_id", beg_label_asym_ids);
            encode_integer_column(pk, "beg_label_seq_id", beg_label_seq_ids);
            encode_string_column(pk, "end_label_asym_id", end_label_asym_ids);
            encode_integer_column(pk, "end_label_seq_id", end_label_seq_ids);
        }
        void encode_chem_comp_bond_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame) {
            const auto& bonds = frame.topology().bonds();
            const auto& bond_orders = frame.topology().bond_orders();

            // If no bonds, write empty category
            if (bonds.empty()) {
                goto pack_empty;
            }

            {
                // Build component-level bond definitions
                // Group bonds by residue and collect unique bonds
                std::vector<std::string> comp_ids;
                std::vector<std::string> atom_id_1s;
                std::vector<std::string> atom_id_2s;
                std::vector<std::string> value_orders;
                std::vector<std::string> pdbx_aromatic_flags;
                std::vector<std::string> pdbx_stereo_configs;
                std::vector<int32_t> pdbx_ordinals;

                // Track unique bonds per residue to avoid duplicates
                std::set<std::tuple<std::string, std::string, std::string>> unique_bonds;
                int32_t ordinal = 1;

                for (size_t bond_idx = 0; bond_idx < bonds.size(); ++bond_idx) {
                    const auto& bond = bonds[bond_idx];
                    size_t atom1_idx = bond[0];
                    size_t atom2_idx = bond[1];

                    // Get bond order
                    Bond::BondOrder bond_order = Bond::UNKNOWN;
                    if (bond_idx < bond_orders.size()) {
                        bond_order = bond_orders[bond_idx];
                    }

                    // Find the residue for these atoms
                    auto residue1_opt = frame.topology().residue_for_atom(atom1_idx);
                    auto residue2_opt = frame.topology().residue_for_atom(atom2_idx);

                    // Only create chem_comp_bond entries for intra-residue bonds
                    if (residue1_opt && residue2_opt) {
                        const auto& residue1 = residue1_opt.value();
                        const auto& residue2 = residue2_opt.value();

                        // Check if they're the same residue (by ID and chain)
                        if (residue1.id() != residue2.id()) {
                            continue;  // Inter-residue bond
                        }

                        auto residue1_chain = residue1.get("chainid");
                        auto residue2_chain = residue2.get("chainid");
                        if (residue1_chain && residue2_chain &&
                            residue1_chain->as_string() != residue2_chain->as_string()) {
                            continue;  // Different chains
                        }

                        std::string comp_id = residue1.name();
                        std::string atom_name_1 = frame[atom1_idx].name();
                        std::string atom_name_2 = frame[atom2_idx].name();

                        // Ensure consistent ordering (lexicographic)
                        if (atom_name_1 > atom_name_2) {
                            std::swap(atom_name_1, atom_name_2);
                        }

                        // Check if this bond definition already exists
                        auto bond_key = std::make_tuple(comp_id, atom_name_1, atom_name_2);
                        if (unique_bonds.find(bond_key) != unique_bonds.end()) {
                            continue;  // Skip duplicate
                        }
                        unique_bonds.insert(bond_key);

                        comp_ids.push_back(comp_id);
                        atom_id_1s.push_back(atom_name_1);
                        atom_id_2s.push_back(atom_name_2);
                        value_orders.push_back(bond_order_to_string(bond_order));

                        // Set aromatic flag based on bond order
                        pdbx_aromatic_flags.push_back(bond_order == Bond::AROMATIC ? "Y" : "N");
                        pdbx_stereo_configs.push_back("N");  // Default
                        pdbx_ordinals.push_back(ordinal++);
                    }
                }

                // If no intra-residue bonds, write empty category
                if (comp_ids.empty()) {
                    goto pack_empty;
                }

                // Category map
                pk.pack_map(2);  // name + columns

                pk.pack("name");
                pk.pack("_chem_comp_bond");

                pk.pack("columns");
                pk.pack_array(7);  // 7 columns

                encode_string_column(pk, "comp_id", comp_ids);
                encode_string_column(pk, "atom_id_1", atom_id_1s);
                encode_string_column(pk, "atom_id_2", atom_id_2s);
                encode_string_column(pk, "value_order", value_orders);
                encode_string_column(pk, "pdbx_aromatic_flag", pdbx_aromatic_flags);
                encode_string_column(pk, "pdbx_stereo_config", pdbx_stereo_configs);
                encode_integer_column(pk, "pdbx_ordinal", pdbx_ordinals);
            }
            return;

        pack_empty : 
            pk.pack_map(2);  // name + columns
            pk.pack("name");
            pk.pack("_chem_comp_bond");
            pk.pack("columns");
            pk.pack_array(0);
            return;
        }

        namespace
        {
            struct EncodeStructConnData
            {
                struct LinkPartner
                {
                std::vector<std::string> auth_asym_ids;
                std::vector<std::string> auth_comp_ids;
                std::vector<int32_t> auth_seq_ids;
                std::vector<std::string> label_atom_ids;
                };
                std::vector<std::string> ids;
                std::vector<std::string> conn_type_ids;
                LinkPartner ptnr1, ptnr2;
                std::vector<std::string> pdbx_value_orders;
            };
            inline bool is_phosphate_bond(
                const std::experimental::optional<const Residue&>& residue1_opt
                , const std::string& atom1_name
                , const std::string& chain1
                , const std::experimental::optional<const Residue&>& residue2_opt
                , const std::string& atom2_name
                , const std::string& chain2
            )
            {
                // Skip phosphate bonds - they'll be created automatically on read
                // Phosphate bond: O3' atom of residue N connects to P atom of residue N+1 on same chain
                bool is_phosphate_bond = false;
                if (residue1_opt && residue2_opt) {
                    const auto& residue1 = residue1_opt.value();
                    const auto& residue2 = residue2_opt.value();
                    auto id1 = residue1.id();
                    auto id2 = residue2.id();

                    // Check if consecutive residues on same chain
                    bool consecutive = (id1 && id2 && (*id2 == *id1 + 1 || *id1 == *id2 + 1));
                    bool same_chain = (chain1 == chain2);

                    if (consecutive && same_chain) {
                        // Check if it's an O3'-P phosphate bond
                        is_phosphate_bond = ((atom1_name == "O3'" && atom2_name == "P") ||
                            (atom1_name == "P" && atom2_name == "O3'"));
                    }
                }
                return is_phosphate_bond;
            }

            inline bool is_peptide_bond(
                const std::experimental::optional<const Residue&>& residue1_opt
                , const std::string& atom1_name
                , const std::string& chain1
                , const std::experimental::optional<const Residue&>& residue2_opt
                , const std::string& atom2_name
                , const std::string& chain2
            )
            {

                // Skip peptide bonds - they'll be created automatically on read
                // Peptide bond: C atom of residue N connects to N atom of residue N+1 on same chain
                bool is_peptide_bond = false;
                if (residue1_opt && residue2_opt) {
                    const auto& residue1 = residue1_opt.value();
                    const auto& residue2 = residue2_opt.value();
                    auto id1 = residue1.id();
                    auto id2 = residue2.id();

                    // Check if consecutive residues on same chain
                    bool consecutive = (id1 && id2 && (*id2 == *id1 + 1 || *id1 == *id2 + 1));
                    bool same_chain = (chain1 == chain2);

                    if (consecutive && same_chain) {
                        // Check if it's a C-N peptide bond
                        is_peptide_bond = ((atom1_name == "C" && atom2_name == "N") ||
                            (atom1_name == "N" && atom2_name == "C"));
                    }
                }
                return is_peptide_bond;
            }

            struct StructConnResidueData
            {
                std::string chain = "A";
                std::string comp_id = "UNK";  // Unknown residue
                int32_t resid = 1;
            };
            inline void extract_data(const std::experimental::optional<const Residue&>& residue_opt, StructConnResidueData& data)
            {
                if (residue_opt) {
                    const auto& residue1 = residue_opt.value();
                    auto chain1_prop = residue1.get("chainid");
                    data.chain = chain1_prop ? chain1_prop->as_string() : "A";
                    data.comp_id = residue1.name();
                    data.resid = residue1.id() ? static_cast<int32_t>(*residue1.id()) : 1;
                }
            }
            inline void append_ptnr(EncodeStructConnData::LinkPartner& ptnr, const StructConnResidueData& data_residue, const std::string& atom_name)
            {
                ptnr.auth_asym_ids.push_back(data_residue.chain);
                ptnr.auth_comp_ids.push_back(data_residue.comp_id);
                ptnr.auth_seq_ids.push_back(data_residue.resid);
                ptnr.label_atom_ids.push_back(atom_name);
            }

            inline void fill(const Frame& frame, EncodeStructConnData& struct_conn_data)
            {
                const auto& bonds = frame.topology().bonds();
                const auto& bond_orders = frame.topology().bond_orders();

                int32_t conn_id = 1;

                for (size_t bond_idx = 0; bond_idx < bonds.size(); ++bond_idx) {
                    const auto& bond = bonds[bond_idx];
                    size_t atom1_idx = bond[0];
                    size_t atom2_idx = bond[1];

                    // Get bond order
                    Bond::BondOrder bond_order = Bond::UNKNOWN;
                    if (bond_idx < bond_orders.size()) {
                        bond_order = bond_orders[bond_idx];
                    }

                    // Find the residues for these atoms
                    auto residue1_opt = frame.topology().residue_for_atom(atom1_idx);
                    auto residue2_opt = frame.topology().residue_for_atom(atom2_idx);

                    // Check if they're in the same residue
                    bool same_residue = false;
                    if (residue1_opt && residue2_opt) {
                        const auto& residue1 = residue1_opt.value();
                        const auto& residue2 = residue2_opt.value();
                        same_residue = (&residue1 == &residue2);
                    }

                    if (same_residue) {
                        continue;  // Intra-residue bond, skip (handled by chem_comp_bond)
                    }

                    // For inter-residue bonds or bonds involving atoms without residues,
                    // write to _struct_conn. Use dummy residue info if needed.
                    std::string atom1_name = frame[atom1_idx].name();
                    std::string atom2_name = frame[atom2_idx].name();

                    StructConnResidueData data_residue1, data_residue2;
                    extract_data(residue1_opt, data_residue1);
                    extract_data(residue2_opt, data_residue2);


                    if (is_peptide_bond(residue1_opt, atom1_name, data_residue1.chain, residue2_opt, atom2_name, data_residue2.chain)) {
                        continue;  // Skip peptide bonds
                    }

                    if (is_phosphate_bond(residue1_opt, atom1_name, data_residue1.chain, residue2_opt, atom2_name, data_residue2.chain)) {
                        continue;  // Skip phosphate bonds
                    }

                    // Add the connection
                    struct_conn_data.ids.push_back("conn" + std::to_string(conn_id++));
                    struct_conn_data.conn_type_ids.push_back("covale");  // Covalent bond

                    append_ptnr(struct_conn_data.ptnr1, data_residue1, atom1_name);
                    append_ptnr(struct_conn_data.ptnr2, data_residue2, atom2_name);

                    struct_conn_data.pdbx_value_orders.push_back(bond_order_to_string(bond_order));
                }

            }

            inline void pack(msgpack::packer<msgpack::sbuffer>& pk, const EncodeStructConnData& struct_conn_data)
            {
                // Category map
                pk.pack_map(2);  // name + columns

                pk.pack("name");
                pk.pack("_struct_conn");

                pk.pack("columns");
                pk.pack_array(10);  // 10 columns

                encode_string_column(pk, "id", struct_conn_data.ids);
                encode_string_column(pk, "conn_type_id", struct_conn_data.conn_type_ids);
                encode_string_column(pk, "ptnr1_auth_asym_id",  struct_conn_data.ptnr1.auth_asym_ids);
                encode_string_column(pk, "ptnr1_auth_comp_id",  struct_conn_data.ptnr1.auth_comp_ids);
                encode_integer_column(pk, "ptnr1_auth_seq_id",  struct_conn_data.ptnr1.auth_seq_ids);
                encode_string_column(pk, "ptnr1_label_atom_id", struct_conn_data.ptnr1.label_atom_ids);
                encode_string_column(pk, "ptnr2_auth_asym_id",  struct_conn_data.ptnr2.auth_asym_ids);
                encode_string_column(pk, "ptnr2_auth_comp_id",  struct_conn_data.ptnr2.auth_comp_ids);
                encode_integer_column(pk, "ptnr2_auth_seq_id",  struct_conn_data.ptnr2.auth_seq_ids);
                encode_string_column(pk, "ptnr2_label_atom_id", struct_conn_data.ptnr2.label_atom_ids);
                encode_string_column(pk, "pdbx_value_order",    struct_conn_data.pdbx_value_orders);
            }
        }
        void encode_struct_conn_category(msgpack::packer<msgpack::sbuffer>& pk, const Frame& frame) {
            const auto& bonds = frame.topology().bonds();

            // If no bonds, write empty category
            if (bonds.empty()) {
                goto pack_empty;
            }
            {
                EncodeStructConnData struct_conn_data;
                fill(frame, struct_conn_data);

                // If no inter-residue bonds, write empty category
                if (struct_conn_data.ids.empty()) {
                    goto pack_empty;
                }

                pack(pk, struct_conn_data);
            }
            return;

        pack_empty: // If we don't have bond data, we build an empty section then return.
            pk.pack_map(2);  // name + columns
            pk.pack("name");
            pk.pack("_struct_conn");
            pk.pack("columns");
            pk.pack_array(0);
            return;
        }

        void encode_float_column(msgpack::packer<msgpack::sbuffer>& pk,
            const std::string& column_name, const std::vector<double>& data) {
            // Column map: { name, mask, data }
            pk.pack_map(3);

            pk.pack("name");
            pk.pack(column_name);

            pk.pack("mask");
            pk.pack_nil();  // No mask

            pk.pack("data");
            // Data map: { data (binary), encoding }
            pk.pack_map(2);

            pk.pack("data");
            // Encode as Float64 ByteArray
            auto binary_data = encode_byte_array_float64(data);
            pk.pack_bin(static_cast<uint32_t>(binary_data.size()));
            pk.pack_bin_body(reinterpret_cast<const char*>(binary_data.data()), static_cast<uint32_t>(binary_data.size()));

            pk.pack("encoding");
            pk.pack_array(1);  // Single encoding: ByteArray
            pk.pack_map(2);  // kind + type
            pk.pack("kind"); pk.pack("ByteArray");
            pk.pack("type");  pk.pack(33);  // Float64 type code
        }

        namespace
        {
            inline void pack_offset_binary(msgpack::packer<msgpack::sbuffer>& pk, std::vector<uint8_t>& offsets_binary)
            {
                int32_t offset_byte_count = 4;
                bool offset_unsigned = true;
                pk.pack_map(4);
                pk.pack("kind"); pk.pack("IntegerPacking");
                pk.pack("byteCount"); pk.pack(offset_byte_count);
                pk.pack("srcSize"); pk.pack(static_cast<int32_t>(offsets_binary.size() / 4));
                pk.pack("isUnsigned"); pk.pack(offset_unsigned);

                pk.pack_map(2);
                pk.pack("kind"); pk.pack("ByteArray");
                pk.pack("type"); pk.pack(4);  // Int32

                // offsets
                pk.pack("offsets");
                pk.pack_bin(static_cast<uint32_t>(offsets_binary.size()));
                pk.pack_bin_body(reinterpret_cast<const char*>(offsets_binary.data()), static_cast<uint32_t>(offsets_binary.size()));
            }
            inline void build_encode_string_data(const std::vector<std::string>& data, std::string& string_data)
            {
                // Build string data (concatenated unique strings)
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
        }
        void encode_string_column(msgpack::packer<msgpack::sbuffer>& pk,
            const std::string& column_name, const std::vector<std::string>& data) {
            // Column map: { name, mask, data }
            pk.pack_map(3);

            pk.pack("name");
            pk.pack(column_name);

            pk.pack("mask");
            pk.pack_nil();  // No mask

            pk.pack("data");
            // Data map: { data (binary), encoding }
            pk.pack_map(2);

            // Encode the string array
            auto encoded_result = encode_string_array(data);
            auto& indices = encoded_result.first;
            auto& offsets_binary = encoded_result.second;

            std::string string_data;
            build_encode_string_data(data, string_data);

            // Apply encoding chain: Delta -> RunLength -> IntegerPacking -> ByteArray
            int32_t origin;
            auto delta_encoded = encode_delta(indices, origin);
            auto rl_encoded = encode_run_length(delta_encoded);
            int32_t byte_count;
            bool is_unsigned;
            auto packed_data = encode_integer_packing(rl_encoded, byte_count, is_unsigned);

            pk.pack("data");
            pk.pack_bin(static_cast<uint32_t>(packed_data.size()));
            pk.pack_bin_body(reinterpret_cast<const char*>(packed_data.data()), static_cast<uint32_t>(packed_data.size()));

            // Encoding specification
            pk.pack("encoding");
            pk.pack_array(1);  // StringArray
            pk.pack_map(5);  // kind, dataEncoding, stringData, offsetEncoding, offsets

            pk.pack("kind");
            pk.pack("StringArray");

            // dataEncoding chain
            pk.pack("dataEncoding");
            pk.pack_array(4);  // Delta, RunLength, IntegerPacking, ByteArray

            // Delta
            pk.pack_map(3);
            pk.pack("kind"); pk.pack("Delta");
            pk.pack("origin"); pk.pack(origin);
            pk.pack("srcType"); pk.pack(3);  // Int32

            // RunLength
            pk.pack_map(3);
            pk.pack("kind"); pk.pack("RunLength");
            pk.pack("srcType"); pk.pack(3);  // Int32
            pk.pack("srcSize"); pk.pack(static_cast<int32_t>(delta_encoded.size()));

            // IntegerPacking
            pk.pack_map(4);
            pk.pack("kind"); pk.pack("IntegerPacking");
            pk.pack("byteCount"); pk.pack(byte_count);
            pk.pack("srcSize"); pk.pack(static_cast<int32_t>(rl_encoded.size()));
            pk.pack("isUnsigned"); pk.pack(is_unsigned);

            // ByteArray
            pk.pack_map(2);
            pk.pack("kind"); pk.pack("ByteArray");
            pk.pack("type"); pk.pack(byte_count == 1 ? 1 : 4);  // Int8 or Int32

            // stringData
            pk.pack("stringData");
            pk.pack(string_data);

            // offsetEncoding
            pk.pack("offsetEncoding");
            pk.pack_array(2);  // IntegerPacking, ByteArray

            pack_offset_binary(pk, offsets_binary);
        }

        void encode_integer_column(msgpack::packer<msgpack::sbuffer>& pk,
            const std::string& column_name, const std::vector<int32_t>& data) {
            // Column map: { name, mask, data }
            pk.pack_map(3);

            pk.pack("name");
            pk.pack(column_name);

            pk.pack("mask");
            pk.pack_nil();  // No mask

            pk.pack("data");
            // Data map: { data (binary), encoding }
            pk.pack_map(2);

            // Apply encoding chain: Delta -> RunLength -> IntegerPacking -> ByteArray
            int32_t origin;
            auto delta_encoded = encode_delta(data, origin);
            auto rl_encoded = encode_run_length(delta_encoded);
            int32_t byte_count;
            bool is_unsigned;
            auto packed_data = encode_integer_packing(rl_encoded, byte_count, is_unsigned);

            pk.pack("data");
            pk.pack_bin(static_cast<uint32_t>(packed_data.size()));
            pk.pack_bin_body(reinterpret_cast<const char*>(packed_data.data()), static_cast<uint32_t>(packed_data.size()));

            // Encoding specification
            pk.pack("encoding");
            pk.pack_array(4);  // Delta, RunLength, IntegerPacking, ByteArray

            // Delta
            pk.pack_map(3);
            pk.pack("kind"); pk.pack("Delta");
            pk.pack("origin"); pk.pack(origin);
            pk.pack("srcType"); pk.pack(3);  // Int32

            // RunLength
            pk.pack_map(3);
            pk.pack("kind"); pk.pack("RunLength");
            pk.pack("srcType"); pk.pack(3);  // Int32
            pk.pack("srcSize"); pk.pack(static_cast<int32_t>(delta_encoded.size()));

            // IntegerPacking
            pk.pack_map(4);
            pk.pack("kind"); pk.pack("IntegerPacking");
            pk.pack("byteCount"); pk.pack(byte_count);
            pk.pack("srcSize"); pk.pack(static_cast<int32_t>(rl_encoded.size()));
            pk.pack("isUnsigned"); pk.pack(is_unsigned);

            // ByteArray
            pk.pack_map(2);
            pk.pack("kind"); pk.pack("ByteArray");
            pk.pack("type"); pk.pack(byte_count == 1 ? 1 : 4);  // Int8 or Int32
        }

    } // end MSGPACK_API_VERSION_NAMESPACE
}

// =============================================================================
// BCIFFormat member function implementations (non-msgpack dependent)
// =============================================================================


namespace chemfiles
{

    BCIFFormat::BCIFFormat(std::string path, File::Mode mode, File::Compression compression)
        : data_(new BCIFData()) 
    {
        if (mode == File::READ) {
            auto file = TextFile(std::move(path), mode, compression);
            auto buffer = file.readall();
            decode(buffer);
            filename_ = file.path();
            mode_ = mode;
        }
        else if (mode == File::WRITE) {
            // For write mode, just store the filename and mode
            // We'll write when write() is called
            filename_ = std::move(path);
            mode_ = mode;
            compression_ = compression;
            has_written_ = false;
        }
        else if (mode == File::APPEND) {
            throw file_error("append mode ('a') is not supported for the BCIF format");
        }
    }
    BCIFFormat::BCIFFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode, File::Compression compression) 
        : data_(new BCIFData()) 
    {
        if (mode == File::WRITE) {
            throw format_error("the BCIF format cannot write to memory");
        }

        memory->decompress(compression);
        std::string data(memory->data(), memory->size());
        decode(data);
        memory_ = memory;
    }

    size_t BCIFFormat::size() {
        return data_->num_models;
    }

    void BCIFFormat::read_at(size_t index, Frame& frame) {
        if (index >= data_->num_models) {
            throw file_error("the BCIF file contains {} models, cannot read model {}", data_->num_models, index);
        }

        model_index_ = index;
        read(frame);
    }

    namespace
    {
        struct DataProfile
        {
            DataProfile(const BCIFFormat::BCIFData& data)
                :
                atom_type_size_ok(data.atom_z.size() == data.atom_type_symbol.size()),
                atom_authLabel_size_ok(data.atom_z.size() == data.auth_atom_label.size()),
                atom_label_size_ok(data.atom_z.size() == data.atom_label.size()),
                atom_id_size_ok(data.atom_z.size() == data.atom_id.size()),
                residue_data_size_ok(data.atom_z.size() == data.residue_name.size()  &&
                   data.atom_z.size() == data.residue_id.size()                      &&
                   data.atom_z.size() == data.chain_id.size()                        &&
                   data.atom_z.size() == data.insertion_code.size()                  &&
                   data.atom_z.size() == data.auth_residue_id.size()                 &&
                   data.atom_z.size() == data.auth_chain_id.size())
            { }

            bool atom_type_size_ok =         false;
            bool atom_authLabel_size_ok =    false;
            bool atom_label_size_ok =        false;
            bool atom_id_size_ok =           false;
            bool residue_data_size_ok =      false;
        };

        inline void create_atoms(const BCIFFormat::BCIFData& data_, Frame& frame)
        {
            // Set up the frame
            const size_t& natoms = data_.atom_x.size();
            frame.resize(natoms);
            auto positions = frame.positions();

            // Add atoms with positions
            for (size_t i = 0; i < natoms; ++i) {
                // Create atom with type symbol and label
                std::string atom_type = "X";  // default
                std::string atom_name = "";

                if (i < data_.atom_type_symbol.size()) {
                    atom_type = data_.atom_type_symbol[i];
                }

                if (i < data_.atom_label.size() && !data_.atom_label[i].empty() && data_.atom_label[i] != "?") {
                    atom_name = data_.atom_label[i];
                }
                else if (i < data_.auth_atom_label.size() && !data_.auth_atom_label[i].empty() && data_.auth_atom_label[i] != "?") {
                    atom_name = data_.auth_atom_label[i];  // fallback to auth if label is empty
                }
                else {
                    atom_name = atom_type;  // final fallback to type
                }

                auto atom = Atom(atom_name, atom_type);
                if (data_.atom_id.size() > i)
                    atom.set("id", data_.atom_id[i]);
                frame[i] = std::move(atom);

                // Set position
                positions[i][0] = data_.atom_x[i];
                positions[i][1] = data_.atom_y[i];
                positions[i][2] = data_.atom_z[i];
            }

        }

        inline void assign_secondary_structure(const BCIFFormat::BCIFData& data_, const std::string& chain_id, const int64_t& residue_id, chemfiles::Residue& residue)
        {
            BCIFFormat::BCIFData::ChainNameResId ss_mapKey{ chain_id, static_cast<uint32_t>(residue_id) };
            if (data_.secondary_structure_map.count(ss_mapKey) > 0)
            {
                residue.set("secondary_structure", data_.secondary_structure_map.at(ss_mapKey));
            }
        }
        using AtomIndex = size_t;
        // Helper function to check if a residue is a nucleotide (RNA or DNA)
        inline bool is_nucleotide(const std::string& residue_name) {
             return (residue_name == "A" || residue_name == "C" || residue_name == "G" || residue_name == "U" ||
                    residue_name == "DA" || residue_name == "DC" || residue_name == "DG" || residue_name == "DT");
        }
        inline bool is_aminoacide(const std::string& residue_name) {
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

        inline void create_intra_residue_bonds(const BCIFFormat::BCIFData::ChemCompMap& chemcomp_map, const std::string& resname, const std::map<BCIFFormat::BCIFData::AtomName, AtomIndex>& atoms, Frame& frame)
        {
            std::unordered_set<std::pair<AtomIndex, AtomIndex>> bounded_atoms; // Avoid bounding the same pair twice

            for (auto& [it_atomName, it_atomIndex] : atoms)
            {
                std::pair< BCIFFormat::BCIFData::ResName, BCIFFormat::BCIFData::AtomName> key{resname, it_atomName };
                auto [begin, end] = chemcomp_map.equal_range(key);
                for (auto it2_bonds = begin; it2_bonds != end; ++it2_bonds)
                {
                    auto& [it2_resname, it2_AtomName1] = it2_bonds->first;
                    auto& [it2_atomName2, it2_bondOrder] = it2_bonds->second;
                    if (atoms.count(it2_atomName2) == 0)
                        continue;

                    std::pair<AtomIndex, AtomIndex> p12(it_atomIndex, atoms.at(it2_atomName2));
                    std::pair<AtomIndex, AtomIndex> p21(atoms.at(it2_atomName2), it_atomIndex);

                    if (bounded_atoms.count(p12) > 0 && bounded_atoms.count(p21))
                        continue;

                    Bond::BondOrder bond_order = parse_bond_order(it2_bondOrder);
                    frame.add_bond(it_atomIndex, atoms.at(it2_atomName2), bond_order);
                    bounded_atoms.insert(std::move(p12));
                    bounded_atoms.insert(std::move(p21));
                }
            }

        }
        inline bool is_residue_forward_binder(const std::string& atomName)
        {
            return atomName == "C" || atomName == "O3'";
        }
        inline bool is_residue_backward_binder(const std::string& atomName)
        {
            return atomName == "N" || atomName == "P";
        }
        inline size_t get_inter_residue_binder(const std::map<BCIFFormat::BCIFData::AtomName, AtomIndex>& atoms)
        {
            for (auto& [it_atomName, it_atomIndex] : atoms)
            {
                if (is_residue_forward_binder(it_atomName))
                    return it_atomIndex;
            }
            return 0xffffffffffffffff;
        }
        inline bool expect_implicit_inter_residue_bonding(const std::string& res_name)
        {
            return (is_nucleotide(res_name) || is_aminoacide(res_name));
        }
        // Check if residue data matches the placeholder pattern used for atoms without residues
        inline bool is_placeholder_residue_data(const std::string& res_name, int32_t res_id, const std::string& chain_id) {
            return res_name == "UNK" && res_id == 1 && chain_id == "A";
        }

        inline void create_residue(
            const BCIFFormat::BCIFData& data,
            Frame& frame,
            const std::string& res_name,
            const int32_t& res_id,
            const std::map<BCIFFormat::BCIFData::AtomName, AtomIndex>& atoms_waiting_for_residue_data,
            const size_t& it_atomIndex,
            size_t& previous_inter_residue_forward_linking_atom,
            size_t& current_inter_residue_forward_linking_atom
        )
        {
            const size_t last_index = it_atomIndex - 1;

            // Skip creating residues for atoms with placeholder residue data
            // These are atoms that didn't belong to any residue in the original file
            if (is_placeholder_residue_data(res_name, res_id, data.chain_id[last_index])) {
                previous_inter_residue_forward_linking_atom = current_inter_residue_forward_linking_atom;
                current_inter_residue_forward_linking_atom = 0xffffffffffffffff;
                return;
            }

            create_intra_residue_bonds(data.chem_comp_bonds_map, res_name, atoms_waiting_for_residue_data, frame);
            Residue residue(res_name, res_id);
            residue.set("chainid", data.chain_id[last_index]);
            residue.set("insertion_code", data.insertion_code[last_index]);
            residue.set("chainname", data.auth_chain_id[last_index]);
            residue.set("auth_seq_id", static_cast<double>(data.auth_residue_id[last_index]));
            for (auto& [_, it_atomIdx] : atoms_waiting_for_residue_data)
            {
                residue.add_atom(it_atomIdx);
            }
            assign_secondary_structure(data, data.chain_id[last_index], data.residue_id[last_index], residue);
            frame.add_residue(std::move(residue));

            previous_inter_residue_forward_linking_atom = current_inter_residue_forward_linking_atom;
            current_inter_residue_forward_linking_atom = 0xffffffffffffffff;

        }
        inline void fill_atomistic_data(const BCIFFormat::BCIFData& data, const DataProfile& profile, Frame& frame)
        {
            const size_t& natoms = data.atom_x.size();
            frame.resize(natoms);
            // TODO : make room in the bond collection for 3*natoms
            auto positions = frame.positions();

            std::map<BCIFFormat::BCIFData::StructConnMapKey, AtomIndex> atoms_waiting_for_struct_conn_bounding;
            std::map<BCIFFormat::BCIFData::AtomName, AtomIndex> atoms_waiting_for_residue_data;
            size_t previous_inter_residue_forward_linking_atom = 0xffffffffffffffff;
            size_t current_inter_residue_forward_linking_atom = 0xffffffffffffffff;
            bool just_changed_chain = false;
            bool just_changed_residue = false;

            const std::string* chain_name = nullptr;
            const std::string*   res_name = nullptr;
            const int32_t*         res_id = nullptr;

            size_t it_atomIndex = 0; // We need the variable to survive after the loop to create the last residue
            for (; it_atomIndex < natoms; ++it_atomIndex) {

                just_changed_chain = it_atomIndex > 0 
                    && profile.residue_data_size_ok 
                    && data.chain_id[it_atomIndex - 1] != data.chain_id[it_atomIndex];
                just_changed_residue = it_atomIndex > 0
                    && (
                        just_changed_chain || (
                            profile.residue_data_size_ok 
                            && (
                                data.residue_id[it_atomIndex - 1] != data.residue_id[it_atomIndex]
                                || data.residue_name[it_atomIndex - 1] != data.residue_name[it_atomIndex]
                                )
                            )
                    );

                // When I wrote the implementation, the version of chemfiles didn't allow to modify a residue once it was added to the frame.
                // To work around it, we create the residue N - 1 once we iterate on the first atom of residue N. 
                if (just_changed_residue)
                {
                    create_residue(data, frame, *res_name, *res_id, atoms_waiting_for_residue_data, it_atomIndex, previous_inter_residue_forward_linking_atom, current_inter_residue_forward_linking_atom);
                    atoms_waiting_for_residue_data.clear();
                    just_changed_residue = false;
                }

                chain_name = nullptr;
                res_name = nullptr;
                res_id = nullptr;

                if (just_changed_chain)
                {
                    // we don't bound residues from different chains
                    previous_inter_residue_forward_linking_atom = 0xffffffffffffffff;
                    just_changed_chain = false;
                }

                if (profile.residue_data_size_ok)
                {
                    chain_name = &data.chain_id[it_atomIndex];
                    res_name = &data.residue_name[it_atomIndex];
                    res_id = &data.residue_id[it_atomIndex];
                }
                bool fetched_all_residue_data = res_name != nullptr && chain_name != nullptr && res_id != nullptr;
                
                std::string atom_type = "X";
                std::string atom_name = "";

                if (profile.atom_type_size_ok)
                    atom_type = data.atom_type_symbol[it_atomIndex];
                if (profile.atom_authLabel_size_ok)
                    atom_name = data.auth_atom_label[it_atomIndex];
                else if (profile.atom_label_size_ok)
                    atom_name = data.atom_label[it_atomIndex];
                else
                    atom_name = atom_type;

                auto atom = Atom(atom_name, atom_type);

                if (profile.atom_id_size_ok)
                    atom.set("id", data.atom_id[it_atomIndex]);

                positions[it_atomIndex][0] = data.atom_x[it_atomIndex];
                positions[it_atomIndex][1] = data.atom_y[it_atomIndex];
                positions[it_atomIndex][2] = data.atom_z[it_atomIndex];

                frame[it_atomIndex] = std::move(atom);
                atoms_waiting_for_residue_data.emplace(std::make_pair(atom_name, it_atomIndex));
                
                if (res_name == nullptr)
                    continue;

                bool current_residue_has_implicit_neightbour_bonding = expect_implicit_inter_residue_bonding(*res_name);
                if (current_inter_residue_forward_linking_atom == 0xffffffffffffffff && current_residue_has_implicit_neightbour_bonding && is_residue_forward_binder(atom_name))
                    current_inter_residue_forward_linking_atom = it_atomIndex;
                if (current_residue_has_implicit_neightbour_bonding && is_residue_backward_binder(atom_name) && previous_inter_residue_forward_linking_atom != 0xffffffffffffffff)
                {
                    frame.add_bond(previous_inter_residue_forward_linking_atom, it_atomIndex, Bond::SINGLE);
                }

                // We create implicit bound between contiguous residues
                if (res_name != nullptr
                    && (*res_name == "N" || *res_name == "P")
                    && previous_inter_residue_forward_linking_atom != 0xffffffffffffffff
                    && current_residue_has_implicit_neightbour_bonding
                    )
                {
                    frame.add_bond(previous_inter_residue_forward_linking_atom, it_atomIndex, Bond::SINGLE);
                }
                if (fetched_all_residue_data && !atom_name.empty())
                {
                    // We store atom index related to some strut_conn bounding for quick access later.
                    BCIFFormat::BCIFData::StructConnMapKey struct_conn_key{*chain_name, *res_name, *res_id, atom_name };
                    if (data.struct_conn_map.count(struct_conn_key) > 0)
                    {
                        atoms_waiting_for_struct_conn_bounding[struct_conn_key] = it_atomIndex;
                    }
                }
            }

            // Since we only create the residue once its last atom is done iterating, we need a last round after the loop.
            if (res_name != nullptr && res_id != nullptr && !atoms_waiting_for_residue_data.empty() && profile.residue_data_size_ok)
                create_residue(data, frame, *res_name, *res_id, atoms_waiting_for_residue_data, it_atomIndex, previous_inter_residue_forward_linking_atom, current_inter_residue_forward_linking_atom);

            for (auto& it_struct_conn : data.struct_conns)
            {
                    BCIFFormat::BCIFData::StructConnMapKey key1{it_struct_conn.ptnr1.label_asym_id, it_struct_conn.ptnr1.label_comp_id, it_struct_conn.ptnr1.label_seq_id, it_struct_conn.ptnr1.label_atom_id};
                    BCIFFormat::BCIFData::StructConnMapKey key2{it_struct_conn.ptnr2.label_asym_id, it_struct_conn.ptnr2.label_comp_id, it_struct_conn.ptnr2.label_seq_id, it_struct_conn.ptnr2.label_atom_id};
                    if (atoms_waiting_for_struct_conn_bounding.count(key1) > 0 && atoms_waiting_for_struct_conn_bounding.count(key2) > 0)
                    {
                        frame.add_bond(atoms_waiting_for_struct_conn_bounding[key1], atoms_waiting_for_struct_conn_bounding[key2]);
                    }
            }
        }

    }
    void BCIFFormat::read(Frame& frame) {
        if (!decoded_) {
            throw file_error("the BCIF file has not been decoded");
        }

        // Check that we have atom position data
        if (data_->atom_x.empty() || data_->atom_y.empty() || data_->atom_z.empty()) {
            throw format_error("BCIF file does not contain atom position data");
        }

        const size_t& natoms = data_->atom_x.size();
        if (natoms != data_->atom_y.size() || natoms != data_->atom_z.size()) {
            throw format_error("BCIF atom coordinate arrays have inconsistent sizes");
        }
        DataProfile profile(*data_);
        fill_atomistic_data(*data_, profile, frame);

        // Set unit cell
        if (data_->cell_length_a > 0.0 && data_->cell_length_b > 0.0 && data_->cell_length_c > 0.0) {
            Vector3D lengths = { data_->cell_length_a, data_->cell_length_b, data_->cell_length_c };
            Vector3D angles = { data_->cell_angle_alpha, data_->cell_angle_beta, data_->cell_angle_gamma };
            frame.set_cell({ lengths, angles });
        }

        // Set PDB ID code if available
        if (!data_->entry_id.empty()) {
            frame.set("pdb_idcode", data_->entry_id);
        }

        // For now, we only support single model
        model_index_++;
    }
    void BCIFFormat::write(const Frame& frame) {
        if (mode_ != File::WRITE) {
            throw file_error("cannot write to BCIF file opened in read mode");
        }

        // BCIF format only supports single frame writes (not multi-frame like trajectories)
        if (has_written_) {
            warning("BCIF", "BCIF format only supports single frame; ignoring subsequent writes");
            return;
        }

        // Encode the frame to MessagePack binary
        auto binary_data = encode(frame);

        // Write to file
        auto file = TextFile(filename_, File::WRITE, compression_);
        file.print("{}", std::string(binary_data.begin(), binary_data.end()));

        has_written_ = true;
    }
   
    namespace
    {
        class DecodeObject
        {
        public:
            using Object = std::remove_const<std::remove_reference<decltype(((msgpack::object_handle*)(nullptr))->get())>::type>::type;
        private:
            const std::string* _data = nullptr;
            msgpack::object_handle _oh;
            Object _obj;
        public:
            DecodeObject(const std::string& data) : _data(&data) {
                msgpack::unpack(_oh, data.data(), data.size());
                _obj = _oh.get();
            }
            DecodeObject(Object&& obj) : _obj(std::move(obj)) {}
            DecodeObject(Object& obj) : _obj(obj) {}

            Object* operator->()
            {
                return &_obj;
            }
            const Object* operator->() const
            {
                return &_obj;
            }
            operator Object&()
            {
                return _obj;
            }
            operator const Object&() const
            {
                return _obj;
            }
        };
        class Task {
        public:
            template <typename T>
            struct Args {
                DecodeObject object;
                std::filesystem::path dataPath;
                const char* column_name;
                std::vector<T>* out = nullptr;
                
            };
        private:
            struct Interface {
                virtual ~Interface() = default;
                virtual void execute() = 0;
            };
            template<typename T>
            class Wrapper : public Interface
            {
                Args<T> _args;
                std::filesystem::path::iterator _target{ _args.dataPath.end()};
                bool _data_extracted = false;

                // Method called recursively to venture into the target path through the tree. Call extract data once the last node is reached.
                void _dive(msgpack::object& src, std::filesystem::path::iterator it_current_location)
                {
                    if (it_current_location == _target)
                    {
                        _extract_data(src);
                        return ;
                    }

                    std::string key = it_current_location->string();
                    std::filesystem::path::iterator next_it = it_current_location; next_it++;

                    if (src.type == msgpack::type::MAP)
                    {
                        auto block_map = src.via.map;
                        for (uint32_t i = 0; i < block_map.size; ++i) {
                            std::string current_key;
                            block_map.ptr[i].key.convert(current_key);
                            if (current_key == key) {
                                _dive(block_map.ptr[i].val, next_it);
                                if (_data_extracted) return ;
                            }
                        }
                    }
                    else if (src.type == msgpack::type::ARRAY)
                    {
                        std::string value{ key };
                        size_t pos = value.find('=');
                        bool return_map_having_keyvalue = false;
                        if (pos != std::string::npos)
                        {
                            key = std::string(value.data(), pos);
                            value = std::string(value.begin() + pos + 1, value.end());
                            return_map_having_keyvalue = true;
                        }

                        auto array = src.via.array;
                        for (uint32_t it_array_index = 0; it_array_index < array.size; ++it_array_index) {
                            if (array.ptr[it_array_index].type != msgpack::type::MAP) {
                                continue;
                            }

                            auto map = array.ptr[it_array_index].via.map;
                            if (return_map_having_keyvalue)
                            {
                                for (uint32_t it_map_index = 0; it_map_index < map.size; ++it_map_index) {
                                    std::string current_key;
                                    std::string current_value;
                                    map.ptr[it_map_index].key.convert(current_key);
                                    if (current_key == key) {
                                        map.ptr[it_map_index].val.convert(current_value);
                                    }
                                    if (current_value == value)
                                    {
                                        _dive(array.ptr[it_array_index], next_it);
                                        if (_data_extracted) return ;
                                    }
                                    else
                                        break;
                                }
                            }
                            else
                            {
                                // Get category name
                                std::string category_name;
                                for (uint32_t j = 0; j < map.size; ++j) {
                                    std::string current_key;
                                    map.ptr[j].key.convert(current_key);
                                    if (current_key == key) {
                                        _dive( map.ptr[j].val, next_it);
                                        return ;
                                    }
                                }
                            }
                        }
                    }

                    return ;
                }

                // Call with the node where we expect the data to be. 
                // We expect the node to be an array of map with name and data node.
                void _extract_data(const DecodeObject& obj)
                {
                    auto columns = obj->via.array;
                    for (uint32_t i = 0; i < columns.size; ++i) {
                        if (columns.ptr[i].type != msgpack::type::MAP) {
                            continue;
                        }

                        std::string column_name;
                        msgpack::object data_obj;
                        msgpack::object mask_obj;
                        bool found_data = false;
                        bool has_mask = false;
                        msgpack::get_name_and_data(columns.ptr[i].via.map, column_name, data_obj, found_data, mask_obj, has_mask);
                        if (column_name != _args.column_name)
                            continue;

                        msgpack::decode_column(data_obj, *_args.out);
                        if constexpr (!std::is_same_v<T, double>)
                        {
                            if (has_mask) {

                                std::vector<int32_t> mask;
                                mask = decode_mask(mask_obj);
                                if (!mask.empty())
                                    *_args.out = msgpack::apply_mask(*_args.out, mask);
                            }
                        }
                        _data_extracted = true;
                        break;
                    }
                }

            public:
                Wrapper(Args<T>&& args) : _args(std::forward<Args<T>>(args)) {}
                virtual void execute() override
                {
                    _dive(_args.object, _args.dataPath.begin());
                }
            };
        public:

            template <typename T>
            Task(Args<T>&& args)
                : _ptr(new Wrapper<T>(std::forward<Args<T>>(args)))
            { }

            void execute()
            {
                if (_ptr) _ptr->execute();
            }

        private:
            std::unique_ptr<Interface> _ptr = nullptr;
        };
        class TaskWorker
        {
        public:
            TaskWorker(Task& task)
                : _thr([task_ptr = &task]() {task_ptr->execute(); })
            { }
            ~TaskWorker()
            {
                if (_thr.joinable())
                {
                    _thr.join();
                }
            }
            TaskWorker(TaskWorker&&) = default;
            TaskWorker& operator=(TaskWorker&&) = default;
            TaskWorker(const TaskWorker&) = delete;
            TaskWorker& operator=(const TaskWorker&) = delete;
        private:
            std::thread _thr;
        };

        void cure_atom_site_data(BCIFFormat::BCIFData& data)
        {

            // Fallback to auth fields when label fields are invalid
            // Some BCIF files (e.g., 1aga) have label_seq_id = -1 for all atoms
            bool label_seq_invalid = !data.residue_id.empty() &&
                std::all_of(data.residue_id.begin(), data.residue_id.end(),
                    [](int32_t id) { return id < 0; });

            if (label_seq_invalid && !data.auth_residue_id.empty()) {
                // Use auth_seq_id when label_seq_id is invalid
                data.residue_id = data.auth_residue_id;
            }

            // Similarly for chain IDs (though less common)
            bool label_chain_invalid = !data.chain_id.empty() &&
                std::all_of(data.chain_id.begin(), data.chain_id.end(),
                    [](const std::string& s) { return s.empty() || s == "?" || s == "."; });

            if (label_chain_invalid && !data.auth_chain_id.empty()) {
                // Use auth_asym_id when label_asym_id is invalid
                data.chain_id = data.auth_chain_id;
            }

        }
    }
    void BCIFFormat::decode(const std::string& data) {

        // Quick note on the types when dealing with [packed] numbers : 
        // I didn't found an "official" table that says what type number correspond to which number type. 
        // However, I was able to guess he following :
        //  1  = Int8
        //  2  = Int16
        //  4  = Int32
        //  5  = Uint8
        //  6  = Uint16
        //  8  = Uint32
        //  32 = Float32
        //  33 = Float64
        try {
            {
                const size_t TASK_NUMBER = 13;
                std::array<Task, TASK_NUMBER> tasks
                {
                    Task(Task::Args<double>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "Cartn_x", &data_->atom_x})
                    , Task(Task::Args<double>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "Cartn_y", &data_->atom_y})
                    , Task(Task::Args<double>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "Cartn_z", &data_->atom_z})
                    , Task(Task::Args<std::string>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "type_symbol", &data_->atom_type_symbol})
                    , Task(Task::Args<std::string>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "label_atom_id", &data_->atom_label})
                    , Task(Task::Args<std::string>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "auth_atom_id", &data_->auth_atom_label})
                    , Task(Task::Args<int32_t>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "id", &data_->atom_id})
                    , Task(Task::Args<std::string>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "label_comp_id", &data_->residue_name})
                    , Task(Task::Args<int32_t>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "label_seq_id", &data_->residue_id})
                    , Task(Task::Args<int32_t>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "auth_seq_id", &data_->auth_residue_id})
                    , Task(Task::Args<std::string>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "label_asym_id", &data_->chain_id})
                    , Task(Task::Args<std::string>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "auth_asym_id", &data_->auth_chain_id})
                    , Task(Task::Args<std::string>{DecodeObject(data), "dataBlocks/categories/name=_atom_site/columns", "pdbx_PDB_ins_code", &data_->insertion_code})
                };
                    
                std::vector<TaskWorker> workers; workers.reserve(TASK_NUMBER);
                for (auto& it_task : tasks)
                    workers.emplace_back(it_task);

            } // We want the workers to finish before the decode is put to true
            cure_atom_site_data(*data_);

            msgpack::object_handle oh;
            msgpack::unpack(oh, data.data(), data.size());

            auto obj = oh.get();

            // BCIF structure: { dataBlocks: [ { header, categories: [ ... ] } ] }
            if (obj.type != msgpack::type::MAP) {
                throw format_error("BCIF root object must be a map");
            }

            // Get dataBlocks array
            msgpack::object data_blocks_obj;
            bool found_data_blocks = false;

            auto root_map = obj.via.map;
            for (uint32_t i = 0; i < root_map.size; ++i) {
                std::string key;
                root_map.ptr[i].key.convert(key);
                if (key == "dataBlocks") {
                    data_blocks_obj = root_map.ptr[i].val;
                    found_data_blocks = true;
                    break;
                }
            }

            if (!found_data_blocks || data_blocks_obj.type != msgpack::type::ARRAY) {
                throw format_error("BCIF must contain a 'dataBlocks' array");
            }

            auto data_blocks = data_blocks_obj.via.array;
            if (data_blocks.size == 0) {
                throw format_error("BCIF dataBlocks array is empty");
            }

            // Parse the first data block (single structure for Phase 1)
            // Call the free function in the msgpack namespace
            msgpack::parse_data_block(data_blocks.ptr[0], *data_);


            decoded_ = true;

        }
        catch (const msgpack::parse_error& e) {
            throw format_error("error while decoding MessagePack from BCIF: '{}'", e.what());
        }
        catch (const msgpack::type_error& e) {
            throw format_error("error while parsing BCIF structure: '{}'", e.what());
        }
        catch (const msgpack::insufficient_bytes& e) {
            throw format_error("error while parsing BCIF structure: '{}'", e.what());
        }
    }

    std::vector<char> BCIFFormat::encode(const Frame& frame) {
        // Build MessagePack structure for BCIF format
        msgpack::sbuffer buffer;
        msgpack::packer<msgpack::sbuffer> pk(&buffer);

        try {
            // Root map: { "dataBlocks": [...] }
            pk.pack_map(1);
            pk.pack("dataBlocks");
            pk.pack_array(1);  // Single data block

            // DataBlock map
            pk.pack_map(2);  // header + categories

            // Header
            pk.pack("header");
            std::string header_str = frame.get("pdb_idcode") ? frame.get("pdb_idcode")->as_string() : "DATA";
            pk.pack(header_str);

            // Categories array
            pk.pack("categories");
            pk.pack_array(6); 

            msgpack::encode_atom_site_category(pk, frame);
            msgpack::encode_cell_category(pk, frame);
            msgpack::encode_struct_conf_category(pk, frame);
            msgpack::encode_struct_sheet_range_category(pk, frame);
            msgpack::encode_chem_comp_bond_category(pk, frame);
            msgpack::encode_struct_conn_category(pk, frame);

            // Convert buffer to vector
            std::vector<char> result(buffer.data(), buffer.data() + buffer.size());
            return result;
        }
        catch (const std::exception& e) {
            throw format_error("error while encoding BCIF MessagePack: '{}'", e.what());
        }
    }
}
