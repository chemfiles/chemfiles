// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <sstream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chemfiles/formats/SDF.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<SDFFormat>() {
    return FormatInfo("SDF").with_extension(".sdf").description(
        "Structural Data File format"
    );
}

/// Fast-forward the file for one step, returning `false` if the file does
/// not contain one more step.
static bool forward(TextFile& file);

SDFFormat::SDFFormat(std::string path, File::Mode mode, File::Compression compression)
    : file_(TextFile::open(std::move(path), mode, compression))
{
    while (!file_->eof()) {
        auto position = file_->tellg();
        if (!file_ || position == std::streampos(-1)) {
            throw format_error("IO error while reading '{}' as SDF", path);
        }
        if (forward(*file_)) {
            steps_positions_.push_back(position);
        }
    }
    file_->rewind();
}

size_t SDFFormat::nsteps() {
    return steps_positions_.size();
}

void SDFFormat::read_step(const size_t step, Frame& frame) {
    assert(step < steps_positions_.size());
    file_->seekg(steps_positions_[step]);
    read(frame);
}

void SDFFormat::read(Frame& frame) {

    // Make this global as it may contain information we need later
    std::string counts_line;
    size_t natoms = 0;
    size_t nbonds = 0;
    try {
        std::string molecule_name = file_->readline();
        frame.set("name", molecule_name);

        file_->readline(); // Program line - skip it
        file_->readline(); // Comment line - skip it

        counts_line = file_->readline();
        natoms = parse<size_t>(counts_line.substr(0, 3));
        nbonds = parse<size_t>(counts_line.substr(3, 3));
    } catch (const std::exception& e) {
        throw format_error("can not read next step as SDF: {}", e.what());
    }

    std::vector<std::string> atom_lines;
    try {
        atom_lines = file_->readlines(natoms);
    } catch (const FileError& e) {
        throw format_error("can not read file: {}", e.what());
    }

    frame.reserve(natoms);
    frame.resize(0);

    for (const auto& line: atom_lines) {
        auto x = parse<double>(line.substr(0, 10));
        auto y = parse<double>(line.substr(10, 10));
        auto z = parse<double>(line.substr(20, 10));
        auto name = trim(line.substr(31, 3));

        frame.add_atom(Atom(name), Vector3D(x, y, z));
    }

    std::vector<std::string> bond_lines;
    try {
        bond_lines = file_->readlines(nbonds);
    } catch (const FileError& e) {
        throw format_error("can not read file: {}", e.what());
    }

    for (const auto& line: bond_lines) {
        auto atom1 = parse<size_t>(line.substr(0, 3));
        auto atom2 = parse<size_t>(line.substr(3, 3));
        auto bondo = parse<size_t>(line.substr(6, 3));

        Bond::BondOrder bo;

        switch (bondo) {
            case 1:
                bo = Bond::SINGLE;
                break;
            case 2:
                bo = Bond::DOUBLE;
                break;
            case 3:
                bo = Bond::TRIPLE;
                break;
            case 4:
                bo = Bond::AROMATIC;
                break;
            case 8: // The 8 ispecifically means unspecified
            default:
                bo = Bond::UNKNOWN;
                break;
        }

        frame.add_bond(atom1 - 1, atom2 - 1, bo);
    }

    // Parsing the file is more or less complete now, but atom properties can
    // still be read (until 'M  END' is reached).
    // This loop breaks when the property block ends or returns on an error
    while(true) {
        try {
            const auto& line = file_->readline();
            if (line.empty()) {
                continue;
            } else if (line.substr(0, 4) == "$$$$") {
                // Ending block, technically wrong - but we can exit safetly
                return;
            } else if (line.substr(0, 6) == "M  END") {
                // Proper end of block
                break;
            } // TODO: Add actual ATOM property parsing here.....
        } catch (const FileError&) {
            // Premature end of file, but we can safetly end here
            warning("Premature end of SDF File during atom property reading!");
            return;
        }
    }

    // This portion of the file is for molecule wide properties.
    // We're done parsing, so just quit if any errors occur
    std::string property_name;
    std::string property_value;
    while(true) {
        try {
            const auto& line = file_->readline();
            if (line.empty()) {
                // This breaks a property group - so store now
                if (property_name.empty()) {
                    warning("Missing property name!");
                    continue;
                }
                frame.set(property_name, Property(property_value));
            } else if (line.substr(0, 4) == "$$$$") {
                // Molecule ending block
                return;
            } else if (line.substr(0, 3) == "> <") {
                // Get the property name
                // It is formated like:
                //> <NAMEGOESHERE>
                const auto npos = line.find_last_of('>');
                property_name = line.substr(3, npos - 3);

                property_value = file_->readline();
            } else {
                // Continuation of a property value
                property_value += '\n';
                property_value += line;
            }
        } catch (const FileError&) {
            warning("Premature end of SDF File during global property reading!");
            return;
        }
    }

}

void SDFFormat::write(const Frame& frame) {
    auto& topology = frame.topology();
    auto& positions = frame.positions();
    assert(frame.size() == topology.size());

    fmt::print(*file_, "{}\n", frame.get<Property::STRING>("name").value_or("NONAME"));
    fmt::print(*file_, " chemfiles-lib\n\n");
    fmt::print(*file_, "{:>3}{:>3}  0     0  0  0  0  0  0999 V2000\n", frame.size(), topology.bonds().size());

    for (size_t i = 0; i < frame.size(); i++) {
        std::string type = topology[i].type();

        if (type.empty() || type.length() > 3) {
            type = "Xxx";
        }

        fmt::print(
            *file_, "{:>10.4f}{:>10.4f}{:>10.4f} {:3} 0  0  0  0  0  0  0  0  0  0  0  0\n",
            positions[i][0], positions[i][1], positions[i][2], type
        );
    }

    for (const auto& bond : topology.bonds()) {

        std::string bond_order;
        auto bo = topology.bond_order(bond[0], bond[1]);

        switch(bo) {
            case Bond::SINGLE:
                bond_order = "  1";
                break;
            case Bond::DOUBLE:
                bond_order = "  2";
                break;
            case Bond::TRIPLE:
                bond_order = "  3";
                break;
            case Bond::AROMATIC:
                bond_order = "  4";
                break;
            case Bond::UNKNOWN:
            default:
                bond_order = "  8";
                break;
        }

        fmt::print(
            *file_, "{:>3}{:>3}{}  0  0  0  0\n",
            bond[0] + 1, bond[1] + 1, bond_order
        );
    }

    fmt::print(*file_, "M END\n$$$$\n");
    steps_positions_.push_back(file_->tellg());
}

bool forward(TextFile& file) {
    if (!file) {return false;}

    size_t natoms = 0;
    size_t nbonds = 0;
    try {
        // Ignore junk lines
        file.readline();
        file.readline();
        file.readline();
        std::string counts_line = file.readline();

        if (counts_line.length() < 10) {
            throw format_error("Counts line must have at least 10 digits, it has {}", counts_line.length());
        }

        natoms = parse<size_t>(counts_line.substr(0,3));
        nbonds = parse<size_t>(counts_line.substr(3,3));
    } catch (const FileError&) {
        // No more line left in the file
        return false;
    } catch (const Error&) {
        // We could not read an integer, so give up here
        return false;
    }

    try {
        file.readlines(natoms + nbonds);
    } catch (const FileError&) {
        // We could not read the lines from the file
        throw format_error(
            "not enough lines in '{}' for SDF format", file.path()
        );
    }

    // Search for ending character, updating the position in the file only
    while (!file.eof()) {
        if (file.readline() == "$$$$") {
            break;
        }
    }

    // We have enough data to parse an entire molecule.
    // So, even if the file does not have an ending string - return true.
    return true;
}
