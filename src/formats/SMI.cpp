// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <set>
#include <cctype>
#include <map>
#include <algorithm>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/SMI.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/periodic_table.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<SMIFormat>() {
    return FormatInfo("SMI").with_extension(".smi").description(
        "SMI text format"
    );
}

/// Fast-forward the file for one step, returning `false` if the file does
/// not contain one more step.
static bool forward(TextFile& file);
/// See if all values in array are true
static bool all(const std::vector<bool>& vec);

SMIFormat::SMIFormat(const std::string& path, File::Mode mode, File::Compression compression)
    : file_(TextFile::open(std::move(path), mode, compression))
{
    while (!file_->eof()) {
        auto position = file_->tellg();
        if (!file_ || position == std::streampos(-1)) {
            throw format_error("IO error while reading '{}' as SMI", path);
        }
        if (forward(*file_)) {
            steps_positions_.push_back(position);
        }
    }
    file_->rewind();
}

size_t SMIFormat::nsteps() {
    return steps_positions_.size() - 1;
}

void SMIFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

/// Attempts to read a number from the string. The `start` argument is updated
/// to the last numberic character in `smiles`.
static size_t read_number(const std::string& smiles, size_t& start) {
    size_t number = 0;
    if (start >= smiles.size() || std::isdigit(smiles[start]) == 0) {
        --start;
        return number;
    }
    do {
        number *= 10;
        number += static_cast<size_t>(smiles[start] - '0');
        ++start;
    } while (start < smiles.size() && (std::isdigit(smiles[start]) != 0));
    --start;
    return number;
}

static std::set<char> aromatic_organic {'b', 'c', 'n', 'o', 's', 'p'};

// Note: Including H here is non-standard
static std::set<std::string> aliphatic_organic {"B", "C", "N", "O", "S", "P",
                                                "F", "Cl", "Br", "I", "H"};

static std::string read_property_atom(const std::string& smiles, size_t& i) {
    auto old = i;
    if (smiles[i] == '\'') {
        do {
             ++i;
        } while( i < smiles.size() && smiles[i] != '\'');
        ++i;
        return smiles.substr(old + 1, i - old - 2);
    } else {
        do {
            ++i;
        } while( i < smiles.size() && std::islower(smiles[i]) != 0);
        return smiles.substr(old, i - old);
    }
}

Atom& SMIFormat::add_atom(Topology& topol, const std::string& atom_name) {
    topol.add_atom(Atom(atom_name));

    if (!first_atom_) {
        topol.add_bond(previous_atom_, ++current_atom_, current_bond_order_);
    }

    first_atom_ = false;
    previous_atom_ = current_atom_;
    current_bond_order_ = Bond::SINGLE;
    return topol[topol.size() - 1];
}

void SMIFormat::process_property_list_(Topology& topol, const std::string& smiles, size_t& i) {
    double mass = 0.0;
    if (std::isdigit(smiles[i]) != 0) {
        mass = static_cast<double>(read_number(smiles, i));
        ++i;
    }

    bool is_aromatic = std::islower(smiles[i]) != 0;
    auto new_atom_name = read_property_atom(smiles, i);
    auto& new_atom = add_atom(topol, new_atom_name);

    if (is_aromatic) {
        new_atom.set("is_aromatic", true);
    }

    if (mass != 0.0) {
        new_atom.set_mass(mass);
    }

    while( i < smiles.size() && smiles[i] != ']') {
        size_t count_or_class = 0;
        std::string chirality_type = "";
        switch (smiles[i]) {
        case 'H':
            count_or_class = read_number(smiles, ++i);
            count_or_class = count_or_class == 0.0 ? 1.0 : count_or_class;
            new_atom.set("explicit_hydrogens", count_or_class);
            break;
        case '-':
            count_or_class = read_number(smiles, ++i);
            count_or_class = count_or_class == 0.0 ? 1.0 : count_or_class;
            new_atom.set_charge(new_atom.charge() - count_or_class);
            break;
        case '+':
            count_or_class = read_number(smiles, ++i);
            count_or_class = count_or_class == 0.0 ? 1.0 : count_or_class;
            new_atom.set_charge(new_atom.charge() + count_or_class);
            break;
        case ':':
            count_or_class = read_number(smiles, ++i);
            new_atom.set("smiles_class", count_or_class);
            break;
        case ']':
            --i;
            break;
        case '@':
            // Set the direction of the chirality
            if (i + 1 < smiles.size() && smiles[i + 1] == '@') {
                chirality_type = "clockwise";
                ++i;
            } else {
                chirality_type = "counter-clockwise";
            }

            // Set the type of the chirality
            if (i + 2 < smiles.size() && smiles.substr(i + 1, 2) == "SP") {
                chirality_type += " square planar";
                i += 2;
            }
            if (i + 2 < smiles.size() && smiles.substr(i + 1, 2) == "TB") {
                chirality_type += " trigonal bipyramidal";
                i += 2;
            }
            if (i + 2 < smiles.size() && smiles.substr(i + 1, 2) == "OH") {
                chirality_type += " octahedral";
                i += 2;
            }
            new_atom.set("chirality", chirality_type);
            break;
        default:
            warning("SMI Reader", "unknown property code: '{}'", smiles[i]);
            break;
        }
        ++i;
    }
}

void SMIFormat::read(Frame& frame) {
    branch_point_ = std::stack<size_t>(); 
    rings_ids_.clear();

    current_atom_ = 0;

    previous_atom_ = 0;
    current_bond_order_ = Bond::SINGLE;
    first_atom_ = true;

    Topology topol;
    std::vector<Residue> molVect;
    molVect.push_back(Residue("mol"));

    auto smiles = file_->readline();

    auto check_ring =
    [&](size_t ring_id) {
        auto ring_lookup = rings_ids_.find(ring_id);

        if (ring_lookup == rings_ids_.end()) {
            rings_ids_.insert({ring_id, {previous_atom_, current_bond_order_}});
            current_bond_order_ = Bond::SINGLE;
            return;
        }

        // Deviation from the standard, technically bond orders need to be equal,
        // but we will accept the stored order if the current order is single.
        // This is common practice
        topol.add_bond(previous_atom_,
            ring_lookup->second.first,
            current_bond_order_ == Bond::SINGLE?
                ring_lookup->second.second :
                current_bond_order_
        );
        rings_ids_.erase(ring_lookup);

        current_bond_order_ = Bond::SINGLE;
    };

    size_t i;
    for (i = 0; i < smiles.size(); ++i) {

        if (topol.size() != 0 && std::isblank(smiles[i]) != 0) {
            break;
        }

        if (smiles[i] == '[') {
            // TODO: Don't pass the full string, instead pass an iterator
            process_property_list_(topol, smiles, ++i);
            continue;
        }

        // We are not in a property list!
        // Therefore, if something is lowercase, then it must be
        // a single character element in the organic subset.
        if (std::islower(smiles[i]) != 0) {
            if (aromatic_organic.count(smiles[i]) == 0) {
                throw format_error("SMI Reader", "element not found: '{}'", smiles[i]);
            }
            add_atom(topol, smiles.substr(i, 1)).set("is_aromatic", true);
            continue;
        }

        // We are not in a property list!
        // Therefore, aliphatic atoms can be written without brackets.
        // Br and Cl are tricky as, but the rest are single character
        if (std::isupper(smiles[i]) != 0) {
            size_t element_length = 1;
            if (smiles[i] == 'C'
                && i + 1 < smiles.size()
                && smiles[i + 1] == 'l') {
                element_length = 2;
            }
            if (smiles[i] == 'B'
                && i + 1 < smiles.size()
                && smiles[i + 1] == 'r') {
                element_length = 2;
            }

            auto element_name = smiles.substr(i, element_length);

            if (aliphatic_organic.count(element_name) == 0) {
                throw format_error("SMI Reader", "bare non-organic atom: '{}'", element_name);    
            }

            add_atom(topol, element_name);
            i += element_length - 1;
            continue;
        }

        if (std::isdigit(smiles[i]) != 0) {
            auto ring_id = static_cast<size_t>(smiles[i] - '0');
            check_ring(ring_id);
            continue;
        }

        switch (smiles[i]) {
        case '.':
            if (!first_atom_) {
                first_atom_ = true;
                current_atom_++;
            }
            molVect.push_back(Residue("mol"));
            break;
        case '/': current_bond_order_ = Bond::UP; break;
        case '\\': current_bond_order_ = Bond::DOWN; break;
        case '~': current_bond_order_ = Bond::UNKNOWN; break;
        case '-': current_bond_order_ = Bond::SINGLE;break;
        case '=': current_bond_order_ = Bond::DOUBLE; break;
        case '#': current_bond_order_ = Bond::TRIPLE; break;
        case '$': current_bond_order_ = Bond::QUADRUPLE; break;
        case ':': current_bond_order_ = Bond::AROMATIC; break;
        case '(':
            branch_point_.push(previous_atom_);
            break;
        case ')':
            previous_atom_ = branch_point_.top();
            branch_point_.pop();
            break;
        case '%': 
            check_ring(read_number(smiles, ++i)); // TODO: Fix this
        break;
        case '*':
            add_atom(topol, "*").set("wildcard", true);
            break;
        case '[':
        case ']':
        case '+':
        case '@':
            warning("SMI Reader", "symbol not allowed outside of property: '{}'", smiles[i]);
            break;
        default:
            warning("SMI Reader", "unknown symbol: '{}'", smiles[i]);
            break;
        }
    }

    for (auto res : molVect) {
        topol.add_residue(std::move(res));
    }

    frame.resize(topol.size());
    frame.set_topology(topol);

    if (i < smiles.size()) {
        auto name = smiles.substr(i);
        frame.set("name", name);
    }
}

/// Depth first search to find all rings.
/// Not gaurenteed to fast, nor efficient, but just to work
/// Also not gaurenteed to find the SSSR, but it will find all rings
/// needed for a given structure to be writen as SMILES
/// \param adj_list The adjacentcy list graph
/// \param hit_atoms Atoms already encountered
/// \param ring_bonds Bonds already known to ring forming bonds
/// \param ring_atoms Map from an atom to the number of rings it forms
/// \param current_atom Atom to be checked in this iteration
/// \param previous_atom Atom which was bound to the current atom
static void find_rings_helper(
    const std::vector<std::vector<size_t>>& adj_list,
    std::vector<bool>& hit_atoms, std::set<Bond>& ring_bonds,
    std::map<size_t, size_t>& ring_atoms,
    size_t current_atom, size_t previous_atom) {

    auto& current_atom_bonds = adj_list[current_atom];
    hit_atoms[current_atom] = true;

    for (auto neighbor : current_atom_bonds) {
        // prevent back tracking
        if (neighbor == previous_atom) {
            continue;
        }

        // We've seen this neighbor before! Ring found
        if (hit_atoms[neighbor]) {
            // Don't process the same ring connection twice
            if (ring_bonds.count(Bond(neighbor, current_atom))) {
                continue;
            }
            ring_bonds.insert(Bond(neighbor, current_atom));

            auto neigh_ring_iter = ring_atoms.find(neighbor);
            if (neigh_ring_iter == ring_atoms.end()) {
                neigh_ring_iter = ring_atoms.insert({neighbor, 0}).first;
            }
            ++neigh_ring_iter->second;

            continue; // No need to see this atom again
        }

        // Continue the search
        find_rings_helper(adj_list, hit_atoms, ring_bonds,
                          ring_atoms, neighbor, current_atom);
    }
}

static std::map<size_t, size_t> find_rings(
    const std::vector<std::vector<size_t>>& adj_list) {

    std::map<size_t, size_t> ring_atoms;
    std::vector<bool> hit_atoms(adj_list.size(), false);
    std::set<Bond> ring_bonds;
    
    while (!all(hit_atoms)) {
        auto not_hit = std::find(hit_atoms.begin(), hit_atoms.end(), false);
        auto current_atom = static_cast<size_t>(std::distance(hit_atoms.begin(), not_hit));
        auto& current_atom_bonds = adj_list[current_atom];
        hit_atoms[current_atom] = true;
        if (current_atom_bonds.empty()) {
            continue;
        }

        find_rings_helper(adj_list, hit_atoms, ring_bonds, ring_atoms,
                          current_atom_bonds[0], current_atom
        );
    }

    return ring_atoms;
}

static void write_atom_smiles(std::unique_ptr<TextFile>& file_, const Atom& atom) {
    bool needs_brackets = false;
    auto type = atom.type();

    double mass;
    size_t mass_int = 0;
    if (std::modf(atom.mass(), &mass) == 0.0 && mass != 0.0) {
        needs_brackets = true;
        mass_int = static_cast<size_t>(mass);
    }

    auto smi_class = atom.get("smiles_class");
    auto chirality = atom.get<Property::STRING>("chirality").value_or("");
    auto explicit_h =atom.get<Property::DOUBLE>("explicit_hydrogens").value_or(0.0);

    double charge;
    int charge_int = 0;
    if (std::modf(atom.charge(), &charge) == 0.0 && charge != 0.0) {
        needs_brackets = true;
        charge_int = static_cast<int>(charge);
    }

    if (explicit_h != 0.0 || smi_class || !chirality.empty() ||
        aliphatic_organic.count(type) == 0) {
        needs_brackets = true;
    }

    auto is_aromatic = atom.get<Property::BOOL>("is_aromatic").value_or(false);

    if (is_aromatic) {
        if (type.size() > 1) {
            needs_brackets = true;
        }
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    }

    if (needs_brackets) {
        fmt::print(*file_, "[");
    }

    if (mass_int != 0) {
        fmt::print(*file_, "{}", mass_int);
    }

    fmt::print(*file_, "{}", type);

    if (smi_class && smi_class->kind() == Property::DOUBLE) {
        fmt::print(*file_, ":{}", static_cast<int>(smi_class->as_double()));
    }

    if (explicit_h == 1.0) {
        fmt::print(*file_, "H");
    }

    if (explicit_h > 1.0) {
        fmt::print(*file_, "H{}", static_cast<int>(explicit_h));
    }

    if (charge_int < 0) {
        fmt::print(*file_, "{}", charge_int);
    }

    if (charge_int > 0) {
        fmt::print(*file_, "+{}", charge_int);
    }

    if (needs_brackets) {
        fmt::print(*file_, "]");
    }
}

void SMIFormat::write_atom(
    const Frame& frame, std::vector<bool>& hit_atoms,
    size_t current_atom, size_t previous_atom) {

    if (hit_atoms[current_atom]) {
        return;
    }

    auto& current_atom_bonds = adj_list_[current_atom];
    hit_atoms[current_atom] = true;

    write_atom_smiles(file_, frame[current_atom]);

    // Prevent prining of additional '('
    size_t ring_start = 0;

    auto any_rings = ring_atoms_.find(current_atom);
    if (any_rings != ring_atoms_.end()) {
        for (size_t i = 0; i < any_rings->second; ++i) {
            ring_count_++;
            ring_start++;
            fmt::print(*file_, "{}", ring_count_);
            ring_stack_.insert({current_atom, ring_count_});
        }
    }

    // Avoid the prining of branch begin/end
    size_t ring_end = 0;

    // Find all ring connections first
    for (auto neighbor : current_atom_bonds) {
        // avoid 'trivial' rings
        if (neighbor == previous_atom) {
            continue;
        }

        // We must have a ring
        if (hit_atoms[neighbor]) {
            auto ring = ring_stack_.find(neighbor);
            if (ring != ring_stack_.end()) {
                fmt::print(*file_, "{}", ring->second);
                ring_stack_.erase(ring);
                ring_end++;
            }
        }
    }

    size_t neighbors_printed = 0;
    for (auto neighbor : current_atom_bonds) {
        // prevent back tracking
        if (neighbor == previous_atom) {
            continue;
        }

        // This got taken care of by printing a ring
        if (hit_atoms[neighbor]) {
            continue;
        }

        // To print a start bracket, we need to be branching (> 2 non-ring bonds)
        // and we don't want to brank the last neighbor printed
        if (neighbors_printed - ring_start < current_atom_bonds.size() - 2 &&
            current_atom_bonds.size() > 2) {
            fmt::print(*file_, "(");
            branch_stack_++;
        }

        // DFS like recursion
        write_atom(frame, hit_atoms, neighbor, current_atom);

        // we printed a neighbor, if there's more than 1 neighbor, then we need to
        // branch for all but the last neighbor
        neighbors_printed++;
    }

    // End of branch
    if (current_atom_bonds.size() - ring_end == 1 && branch_stack_ != 0) {
        fmt::print(*file_, ")");
        branch_stack_--;
    }
}

void SMIFormat::write(const Frame& frame) {

    if (frame.size() == 0) {
        fmt::print(*file_, "\n");
        return;
    }

    if (frame.size() == 1) {
        write_atom_smiles(file_, frame[0]);
        fmt::print(*file_, "\n");
        return;
    }

    // Create an adjacent list as working with this is easier
    adj_list_.clear();
    adj_list_.resize(frame.size());
    for (auto& bond : frame.topology().bonds()) {
        adj_list_[bond[0]].push_back(bond[1]);
        adj_list_[bond[1]].push_back(bond[0]);
    }

    ring_atoms_ = find_rings(adj_list_);

    std::vector<bool> written(frame.size(), false);
    branch_stack_ = 0;

    ring_stack_.clear();
    ring_count_ = 0;

    first_atom_= true;

    while (!all(written)) {
        if (!first_atom_) {
            fmt::print(*file_, ".");
        }
        auto not_hit = std::find(written.begin(), written.end(), false);
        auto current_atom = static_cast<size_t>(std::distance(written.begin(), not_hit));
        write_atom(frame, written, current_atom, 0);
        first_atom_ = false;
    }

    fmt::print(*file_, "\n");
}

bool forward(TextFile& file) {
    if (!file) {return false;}

    try {
        auto line = file.readline();
    } catch (const FileError&) {
        // No more line left in the file
        return false;
    }
    
    return true;
}

bool all(const std::vector<bool>& vec) {
    for (auto i : vec) {
        if (!i) {
            return false;
        }
    }

    return true;
}
