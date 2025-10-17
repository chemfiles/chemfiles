// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_BCIF_HPP
#define CHEMFILES_FORMAT_BCIF_HPP

#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"

namespace chemfiles {
class Frame;
class UnitCell;
class MemoryBuffer;
class FormatMetadata;

/// BinaryCIF (BCIF) file format reader.
///
/// BinaryCIF is a binary encoding of mmCIF files using MessagePack as the
/// container format with additional compression schemes.
class BCIFFormat final: public Format {
public:
    struct BCIFData {
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

        // Secondary structure data (from _struct_conf category)
        // Maps (chain_id, beg_seq_id) -> (chain_id, end_seq_id, conf_type_id)
        using SecondaryStructureKey = std::pair<std::string, int32_t>;  // chain, resid
        using SecondaryStructureRange = std::tuple<std::string, int32_t, std::string>;  // end_chain, end_resid, type
        std::map<SecondaryStructureKey, SecondaryStructureRange> secondary_structures;

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

        // Bond data (from _struct_conn category)
        // Instance-specific bonds (inter-residue, metal coordination, disulfide, etc.)
        struct StructConn {
            std::string conn_type_id;             // Type of connection (covale, disulf, metalc, etc.)
            std::string ptnr1_label_asym_id;      // Chain ID for atom 1
            std::string ptnr1_label_comp_id;      // Residue name for atom 1
            int32_t ptnr1_label_seq_id;           // Residue ID for atom 1
            std::string ptnr1_label_atom_id;      // Atom name for atom 1
            std::string ptnr2_label_asym_id;      // Chain ID for atom 2
            std::string ptnr2_label_comp_id;      // Residue name for atom 2
            int32_t ptnr2_label_seq_id;           // Residue ID for atom 2
            std::string ptnr2_label_atom_id;      // Atom name for atom 2
            std::string pdbx_value_order;         // Bond order
        };
        std::vector<StructConn> struct_conns;
    };

    BCIFFormat(std::string path, File::Mode mode, File::Compression compression);
    BCIFFormat(std::shared_ptr<MemoryBuffer> memory, File::Mode mode, File::Compression compression);


    void read_at(size_t index, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t size() override;

private:
    /// Perform the BCIF decoding and parsing
    void decode(const std::string& data);

    /// Perform the BCIF encoding for writing
    std::vector<char> encode(const Frame& frame);

    /// Storage for the parsed BCIF data
    BCIFData  data_;

    /// Current model being read. Ranges from [0, data_.num_models)
    size_t model_index_ = 0;

    /// Has the file been decoded yet?
    bool decoded_ = false;

    /// Location of BCIF file on disk
    std::string filename_;

    /// Memory buffer (for memory-based reading)
    std::shared_ptr<MemoryBuffer> memory_;

    /// File mode (READ or WRITE)
    File::Mode mode_ = File::READ;

    /// Compression for writing
    File::Compression compression_ = File::DEFAULT;

    /// Whether any frames have been written
    bool has_written_ = false;
};

template<> const FormatMetadata& format_metadata<BCIFFormat>();

} // namespace chemfiles

#endif
