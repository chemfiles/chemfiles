// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_SMI_HPP
#define CHEMFILES_FORMAT_SMI_HPP

#include <stack>
#include <map>

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Connectivity.hpp"
#include "chemfiles/Topology.hpp"

namespace chemfiles {

/// [SMI] file format reader and writer.
///
/// [SMI]: http://opensmiles.org/opensmiles.html
class SMIFormat final: public Format {
public:
    SMIFormat(const std::string& path, File::Mode mode, File::Compression compression);

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;
private:
    /// Text file where we read from
    std::unique_ptr<TextFile> file_;
    /// Storing the positions of all the steps in the file, so that we can
    /// just `seekg` them instead of reading the whole step.
    std::vector<std::streampos> steps_positions_;

    /// [for reading] Stores location of a branching path
    std::stack<size_t> branch_point_;

    /// [for reading] Stores a mapping between a ring ID and the atom which
    /// starts the ring and a stored bond order
    std::map <size_t, std::pair<size_t, Bond::BondOrder>> rings_ids_;

    /// [for reading] The current atom being added (active atom)
    size_t current_atom_;

    /// [for reading] The previous active atom
    size_t previous_atom_;

    /// [for reading] The current bond ordeer
    Bond::BondOrder current_bond_order_;

    /// [for reading] The
    std::vector<Residue> mol_vector_;

    /// [for reading] Should we connect the previous atom to the first atom
    /// [for writing] Should we add a '.' after the current molecule
    bool first_atom_;

    /// [for reading] adds an atom defined by `atom_name` to the topology
    Atom& add_atom(Topology& topo, const std::string& atom_name);

    /// [for reading] adds an atom defined by the string `smiles` starting at position i
    void process_property_list_(Topology& topo, const std::string& smiles, size_t& i);

    /// [for writing] stores the graph of the topology
    std::vector<std::vector<size_t>> adj_list_;

    /// [for writing] stores how many branches need to be closed
    size_t branch_stack_;

    /// [for writing] stores locations of ring closures
    std::multimap<size_t, size_t> ring_stack_;

    /// [for writing] stores how many rings we need to close
    size_t ring_count_;

    /// [for writing] stores how many rings each atom is in
    std::map<size_t, size_t> ring_atoms_;

    /// [for writing] writes a single atom to the internal file
    /// this function is recursive
    void write_atom(const Frame& frame,
        std::vector<bool>& hit_atoms,
        size_t current_atom, size_t previous_atom);
};

template<> FormatInfo format_information<SMIFormat>();

} // namespace chemfiles

#endif
