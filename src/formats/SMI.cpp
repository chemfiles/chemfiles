// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cmath>
#include <cstdlib>
#include <cctype>

#include <set>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <memory>
#include <istream>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <fmt/ostream.h>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/Connectivity.hpp"

#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/formats/SMI.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<SMIFormat>() {
    return FormatInfo("SMI").with_extension(".smi").description(
        "SMI text format"
    );
}

/// See if all values in array are true
static bool all(const std::vector<bool>& vec);

static std::unordered_set<char> AROMATIC_ORGANIC {'b', 'c', 'n', 'o', 's', 'p'};

// Note: Including H here is non-standard, but allows for HC(H)(H)H
static std::unordered_set<std::string> ALIPHATIC_ORGANIC {"B", "C", "N", "O", "S", "P",
                                                          "F", "Cl", "Br", "I", "H"};

// Valid chirality tags
static std::set<std::string> CHIRALITY_TAGS {"TH", "SP", "TB", "OH", "AL"};

/// Attempts to read a number from the string. The `start` argument is updated
/// to the last numberic character in `smiles`.
static size_t read_number(const std::string& smiles, size_t& start) {
    if (start >= smiles.size() || std::isdigit(smiles[start]) == 0) {
        start--;
        return 0;
    }
    size_t old = start;
    do {
        start++;
    } while (start < smiles.size() && (std::isdigit(smiles[start]) != 0));
    start--;
    return parse<size_t>(smiles.substr(old, start - old + 1));
}

static std::string read_property_atom(const std::string& smiles, size_t& start) {
    auto old = start;
    if (smiles[start] == '\'') {
        do {
             start++;
        } while( start < smiles.size() && smiles[start] != '\'');
        start++;
        return smiles.substr(old + 1, start - old - 2);
    } else {
        do {
            start++;
        } while( start < smiles.size() && std::islower(smiles[start]) != 0);
        return smiles.substr(old, start - old);
    }
}

Atom& SMIFormat::add_atom(Topology& topol, std::string atom_name) {
    topol.add_atom(Atom(std::move(atom_name)));

    if (!first_atom_) {
        topol.add_bond(previous_atom_, ++current_atom_, current_bond_order_);
    }

    first_atom_ = false;
    previous_atom_ = current_atom_;
    current_bond_order_ = Bond::SINGLE;
    mol_vector_.back().add_atom(topol.size() - 1);
    return topol[topol.size() - 1];
}

void SMIFormat::process_property_list_(Topology& topol, const std::string& smiles, size_t& i) {
    double mass = 0.0;
    if (std::isdigit(smiles[i]) != 0) {
        mass = static_cast<double>(read_number(smiles, i));
        ++i;
    }

    bool is_aromatic = std::islower(smiles[i]) != 0;
    auto name = read_property_atom(smiles, i);
    auto& new_atom = add_atom(topol, std::move(name));

    if (is_aromatic) {
        new_atom.set("is_aromatic", true);
    }

    if (mass != 0.0) {
        new_atom.set_mass(mass);
    }

    while( i < smiles.size() && smiles[i] != ']') {
        size_t count_or_class = 0;
        std::string chirality_type = "CCW";
        switch (smiles[i]) {
        case 'H':
            count_or_class = read_number(smiles, ++i);
            count_or_class = (count_or_class == 0) ? 1 : count_or_class;
            new_atom.set("hydrogen_count", count_or_class);
            break;
        case '-':
            count_or_class = read_number(smiles, ++i);
            count_or_class = (count_or_class == 0) ? 1 : count_or_class;
            new_atom.set_charge(new_atom.charge() - static_cast<double>(count_or_class));
            break;
        case '+':
            count_or_class = read_number(smiles, ++i);
            count_or_class = (count_or_class == 0) ? 1 : count_or_class;
            new_atom.set_charge(new_atom.charge() + static_cast<double>(count_or_class));
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
                chirality_type = "CW";
                ++i;
            } else if (i + 2 < smiles.size() &&
                       CHIRALITY_TAGS.count(smiles.substr(i + 1, 2)) != 0) {
                // Set the type of the chirality
                chirality_type += " " + smiles.substr(i + 1, 2);
                i += 2;
                chirality_type += std::to_string(read_number(smiles, ++i));
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

void SMIFormat::read_next(Frame& frame) {
    // Initialize all the reading variables
    branch_point_ = std::stack<size_t, std::vector<size_t>>();
    rings_ids_.clear();
    mol_vector_.clear();
    current_atom_ = 0;
    previous_atom_ = 0;
    current_bond_order_ = Bond::SINGLE;
    first_atom_ = true;

    Topology topol;
    size_t groupid = 1;
    mol_vector_.push_back(Residue("group " + std::to_string(groupid)));

    auto smiles = trim(file_->readline());
    while (smiles.empty()) {
        smiles = trim(file_->readline());
    }

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

        if (std::isblank(smiles[i]) != 0) {
            break;
        }

        if (smiles[i] == '[') {
            // TODO: Don't pass the full string, instead pass an iterator
            // or maybe a substring till the closing ']'
            process_property_list_(topol, smiles, ++i);
            continue;
        }

        // We are not in a property list!
        // Therefore, if something is lowercase, then it must be
        // a single character element in the organic subset.
        if (std::islower(smiles[i]) != 0) {
            if (AROMATIC_ORGANIC.count(smiles[i]) == 0) {
                throw format_error("SMI Reader", "aromatic element not found: '{}'", smiles[i]);
            }
            auto name = smiles.substr(i, 1);
            add_atom(topol, std::move(name)).set("is_aromatic", true);
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

            if (ALIPHATIC_ORGANIC.count(element_name) == 0) {
                throw format_error("SMI Reader", "bare non-organic atom: '{}'", element_name);
            }

            add_atom(topol, std::move(element_name));
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
            mol_vector_.push_back(Residue("group " + std::to_string(groupid)));
            break;
        case '>': ++groupid; break;
        case '/': current_bond_order_ = Bond::UP; break;
        case '\\': current_bond_order_ = Bond::DOWN; break;
        case '~': current_bond_order_ = Bond::UNKNOWN; break;
        case '-':
            if (i + 1 < smiles.size() && smiles[i] == '>') {
                current_bond_order_ = Bond::DATIVE_R;
                ++i;
            } else {
                current_bond_order_ = Bond::SINGLE;
            }
            break;
        case '<':
            if (i + 1 < smiles.size() && smiles[i] == '-') {
                current_bond_order_ = Bond::DATIVE_L;
                ++i;
            } else {
                ++groupid; // Allow for reaction smiles, otherwise it's an unknown character
                first_atom_ = true;
            }
            break;
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
            if (i + 2 >= smiles.size()) {
                throw format_error("SMI Reader", "rings defined with '%' must be double digits");
            }
            check_ring(parse<size_t>(smiles.substr(i + 1, 2)));
            break;
        case '*':
            add_atom(topol, "*").set("wildcard", true);
            break;
        case '[':
        case ']':
        case '+':
        case '@':
            throw format_error("SMI Reader", "symbol not allowed outside of property: '{}'", smiles[i]);
            break;
        default:
            throw format_error("SMI Reader", "unknown symbol: '{}'", smiles[i]);
            break;
        }
    }

    for (auto res : mol_vector_) {
        topol.add_residue(std::move(res));
    }

    if (!branch_point_.empty()) {
        throw format_error("SMI Reader", "{} unclosed '('(s)", branch_point_.size());
    }

    if (!rings_ids_.empty()) {
        throw format_error("SMI Reader", "{} unclosed ringid '{}'", rings_ids_.begin()->first);
    }

    frame.resize(topol.size());
    frame.set_topology(topol);

    if (i < smiles.size()) {
        auto name = smiles.substr(i);
        frame.set("name", trim(name));
    }
}

/// Depth first search to find all rings.
/// Not guaranteed to fast, nor efficient, but just to work
/// Also not guaranteed to find the SSSR, but it will find all rings
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
    std::unordered_map<size_t, size_t>& ring_atoms,
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
            neigh_ring_iter->second++;

            continue; // No need to see this atom again
        }

        // Continue the search
        find_rings_helper(adj_list, hit_atoms, ring_bonds, ring_atoms, neighbor, current_atom);
    }
}

static void find_rings(
    const std::vector<std::vector<size_t>>& adj_list,
    std::unordered_map<size_t, size_t>& ring_atoms) {

    ring_atoms.clear();
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

        find_rings_helper(adj_list, hit_atoms, ring_bonds, ring_atoms, current_atom_bonds[0], current_atom);
    }
}

/// Sets int_part to `value` if `value` is actually an integer
static bool double_to_int(double value, int& int_part) {
    double integer_section;
    auto remainder = std::modf(value, &integer_section);
    if (remainder == 0.0) {
        int_part = static_cast<int>(std::floor(integer_section));
        return true;
    }
    return false;
}

static void write_atom_smiles(std::unique_ptr<TextFile>& file_, const Atom& atom) {
    bool needs_brackets = false;
    auto type = atom.type();

    // The mass must be an integer is the only check we need as all atoms in the
    // periodic table have non-integer masses. Therefore, if the the mass is
    // an integer, then we know the user has set an isotope.
    int mass_int = 0;
    if (double_to_int(atom.mass(), mass_int)) {
        needs_brackets = true;
    }

    // If any of these values are set / not a default value
    auto chirality = atom.get<Property::STRING>("chirality").value_or("");
    if (!chirality.empty()) {
        needs_brackets = true;
    }

    int smi_class = -1;
    auto smi_class_prop = atom.get("smiles_class");
    if (smi_class_prop && smi_class_prop->kind() == Property::DOUBLE &&
        double_to_int(smi_class_prop->as_double(), smi_class) && smi_class >= 0) {
        needs_brackets = true;
    } else if (smi_class_prop) {
        warning("SMI Writer", "the 'smiles_class' property must be an integer >= 0");
    }

    int explicit_h = -1;
    auto explicit_h_prop = atom.get("hydrogen_count");
    if (explicit_h_prop && explicit_h_prop->kind() == Property::DOUBLE &&
        double_to_int(explicit_h_prop->as_double(), explicit_h) && explicit_h >= 0) {
        needs_brackets = true;
    } else if (explicit_h_prop) {
        warning("SMI Writer", "the 'hydrogen_count' property must be an integer >= 0");
    }

    // Charge is similar to mass. It must be an integer, otherwise it is ignored
    int charge_int = 0;
    if (double_to_int(atom.charge(), charge_int)) {
        // Use |= because we don't want to unset if charge_int is 0
        needs_brackets |= (charge_int != 0);
    }

    // Before the atom is printed, we must know if we are printing a property set.
    if (ALIPHATIC_ORGANIC.count(type) == 0) {
        needs_brackets = true;
    }

    auto is_aromatic = atom.get<Property::BOOL>("is_aromatic").value_or(false);

    if (is_aromatic) {
        // We need to print brackets for aromatic Te, Se, As, and Si
        // Note, we allow aromatic Boron to be bare (no brackets) in following
        // ChemAxon's standard.
        if (type.size() > 1) {
            needs_brackets = true;
        }
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    }

    if (needs_brackets) {
        fmt::print(*file_, "[");
    }

    // Mass must be first, before the element is printed
    if (mass_int != 0) {
        fmt::print(*file_, "{}", mass_int);
    }

    fmt::print(*file_, "{}", type);

    if (smi_class != -1) {
        fmt::print(*file_, ":{}", smi_class);
    }

    bool is_good_tag = false;
    switch(chirality.size()) {
    case 0:
        is_good_tag = true;
        break;
    case 2:
        // If it's clockwise, then we just print the symbol @@ and be done with it
        if (chirality == "CW") {
            is_good_tag = true;
            fmt::print(*file_, "@@");
        }
        break;
    case 3:
        if (chirality == "CCW") {
            is_good_tag = true;
            fmt::print(*file_, "@");
        }
        break;
    case 7:
        if (chirality.find("CCW") == 0 &&
            CHIRALITY_TAGS.count(chirality.substr(4, 2)) != 0 &&
            std::isdigit(chirality[6]) != 0 ) {
            is_good_tag = true;
            fmt::print(*file_, "@{}", chirality.substr(4));
        }
        break;
    case 8:
        if (chirality.find("CCW") == 0 &&
            CHIRALITY_TAGS.count(chirality.substr(4, 2)) != 0 &&
            std::isdigit(chirality[6]) != 0 &&
            std::isdigit(chirality[7]) != 0) {
            is_good_tag = true;
            fmt::print(*file_, "@{}", chirality.substr(4));
        }
        break;
    default:
        break;
    }

    if (!is_good_tag) {
        warning("SMI Writer", "invalid chirality tag '{}'", chirality);
    }

    if (explicit_h == 1) {
        fmt::print(*file_, "H");
    }

    if (explicit_h > 1) {
        fmt::print(*file_, "H{}", explicit_h);
    }

    if (charge_int < 0) {
        fmt::print(*file_, "-", charge_int);
    }

    if (charge_int > 0) {
        fmt::print(*file_, "+", charge_int);
    }

    charge_int = std::abs(charge_int);
    if (charge_int != 1 && charge_int != 0) {
        fmt::print(*file_, "{}", charge_int);
    }

    if (needs_brackets) {
        fmt::print(*file_, "]");
    }
}

static void print_bond(std::unique_ptr<TextFile>& file_, chemfiles::Bond::BondOrder bo) {
    switch(bo) {
    case Bond::SINGLE:
    case Bond::AMIDE:
        break;
    case Bond::DOUBLE: fmt::print(*file_, "="); break;
    case Bond::TRIPLE: fmt::print(*file_, "#"); break;
    case Bond::QUADRUPLE: fmt::print(*file_, "$"); break;
    case Bond::AROMATIC: fmt::print(*file_, ":"); break;
    case Bond::DATIVE_L: fmt::print(*file_, "<-"); break;
    case Bond::DATIVE_R: fmt::print(*file_, "->"); break;
    case Bond::UP: fmt::print(*file_, "/"); break;
    case Bond::DOWN: fmt::print(*file_, "\\"); break;
    case Bond::UNKNOWN:
        fmt::print(*file_, "~");
        break;
    default:
        warning("SMI Writer", "unknown bond type");
        fmt::print(*file_, "~");
        break;
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

    if (current_atom != previous_atom) {
        print_bond(file_,
                   frame.topology().bond_order(previous_atom, current_atom)
        );
    }
    write_atom_smiles(file_, frame[current_atom]);

    // Prevent printing of additional '('
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
                print_bond(file_,
                    frame.topology().bond_order(current_atom, neighbor)
                );
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

        // Depth First Search like recursion
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

void SMIFormat::write_next(const Frame& frame) {
    if (frame.size() == 0) {
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

    find_rings(adj_list_, ring_atoms_);

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
        write_atom(frame, written, current_atom, current_atom);
        first_atom_ = false;
    }

    auto name = frame.get("name");
    if (name && name->kind() == Property::STRING) {
        fmt::print(*file_, "\t{}", name->as_string());
    }

    fmt::print(*file_, "\n");
}

std::streampos SMIFormat::forward() {
    if (!*file_) {
        return std::streampos(-1);
    }

    auto position = file_->tellg();
    try {
        auto line = file_->readline();
        while (trim(line).empty()) {
            line = file_->readline();
        }
    } catch (const FileError&) {
        // No more line left in the file
        return std::streampos(-1);
    }

    return position;
}

bool all(const std::vector<bool>& vec) {
    for (auto i : vec) {
        if (!i) {
            return false;
        }
    }

    return true;
}
