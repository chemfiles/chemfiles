// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <set>
#include <cctype>

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
    //molVect.back().add_atom(topol.size() - 1);
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

void SMIFormat::write(const Frame& frame) {
    // Create a bond map as working with this is easier
    std::vector<std::vector<size_t>> bonds(frame.size());
    for (auto& bond : frame.topology().bonds()) {
        bonds[bond[0]].push_back(bond[1]);
        bonds[bond[1]].push_back(bond[0]);
    }

    //TODO: Use residues
    std::vector<bool> written(frame.size(), false);
    size_t next_atom = 0;
    std::stack<size_t> branch_points;
    //std::queue<size_t> rings;

    while (!all(written)) {
        const auto& current_bonds = bonds[next_atom];

        fmt::print(*file_, frame[next_atom].type());
        written[next_atom] = true;

        if (current_bonds.size() == 0) {
            break;
        } else if (current_bonds.size() == 1) { //End of a branch point
            if (written[current_bonds[0]]) { // We are done with a branch
                if (branch_points.size()) {
                    next_atom = branch_points.top();
                    branch_points.pop();
                    fmt::print(*file_, ")");
                } else {
                    break;
                }
            } else {
                next_atom = current_bonds[0];
            }
        } else if (current_bonds.size() == 2 && next_atom == 0) {
            branch_points.push(current_bonds[1]);
            next_atom = current_bonds[0];
        } else {
            fmt::print(*file_, "(");
            bool selected = false;
            for (const auto& neighbor : current_bonds) {
                if (!written[neighbor]) {
                    if (!selected) {
                        selected = true;
                        next_atom = neighbor;
                    } else {
                        branch_points.push(neighbor);
                    }
                }
            }
        }
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
