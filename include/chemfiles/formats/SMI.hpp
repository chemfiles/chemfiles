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

    /// Stores location of a branching path
    std::stack<size_t> branch_point_;

    /// Stores a mapping between a ring ID and the atom which starts the ring
    /// and a stored bond order
    std::map <size_t, std::pair<size_t, Bond::BondOrder>> rings_ids_;

    /// The current atom being added (active atom)
    size_t current_atom_;

    /// The previous active atom
    size_t previous_atom_;

    /// The current bond ordeer
    Bond::BondOrder current_bond_order_;

    /// Should we connect the previous atom to the first atom
    bool first_atom_;

    Atom& add_atom(Topology& topo, const std::string& atom_name);

    void process_property_list_(Topology& topo, const std::string& smiles, size_t& i);
};

template<> FormatInfo format_information<SMIFormat>();

} // namespace chemfiles

#endif
