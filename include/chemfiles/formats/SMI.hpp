// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_SMI_HPP
#define CHEMFILES_FORMAT_SMI_HPP

#include <cstdint>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <unordered_map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Connectivity.hpp"
#include "chemfiles/string_view.hpp"

namespace chemfiles {
class Atom;
class Frame;
class Topology;


/// [SMI] file format reader and writer.
///
/// [SMI]: http://opensmiles.org/opensmiles.html
class SMIFormat final: public TextFormat {
public:
    SMIFormat(const std::string& path, File::Mode mode, File::Compression compression):
        TextFormat(std::move(path), mode, compression) {}

    void read_next(Frame& frame) override;
    void write_next(const Frame& frame) override;
    optional<uint64_t> forward() override;

private:
    /// [for reading] adds an atom defined by `atom_name` to the topology
    Atom& add_atom(Topology& topo, string_view atom_name);

    /// [for reading] adds an atom defined by the string `smiles` starting at position i
    void process_property_list(Topology& topo, string_view smiles, size_t& i);

    /// [for reading] Stores location of a branching path
    std::stack<size_t, std::vector<size_t>> branch_point_;

    /// [for reading] Stores a mapping between a ring ID and the atom which
    /// starts the ring and a stored bond order
    std::unordered_map <size_t, std::pair<size_t, Bond::BondOrder>> rings_ids_;

    /// [for reading] The current atom being added (active atom)
    size_t current_atom_;

    /// [for reading] The previous active atom
    size_t previous_atom_;

    /// [for reading] The current bond ordeer
    Bond::BondOrder current_bond_order_;

    /// [for reading] List of groups
    std::vector<Residue> residues_;

    /// [for reading] Should we connect the previous atom to the first atom
    /// [for writing] Should we add a '.' after the current molecule
    bool first_atom_;

    /// [for writing] writes a single atom to the internal file
    /// this function is recursive
    void write_atom(const Frame& frame,
        std::vector<bool>& hit_atoms,
        size_t current_atom, size_t previous_atom);

    /// [for writing] stores the graph of the topology
    std::vector<std::vector<size_t>> adj_list_;

    /// [for writing] stores how many branches need to be closed
    size_t branch_stack_;

    /// [for writing] stores locations of ring closures. This is ordered
    /// to ensure rings get printed in numeric order (where possible)
    std::multimap<size_t, size_t> ring_stack_;

    /// [for writing] stores how many rings we need to close
    size_t ring_count_;

    /// [for writing] stores how many rings each atom is in
    std::unordered_map<size_t, size_t> ring_atoms_;
};

template<> FormatInfo format_information<SMIFormat>();

} // namespace chemfiles

#endif
