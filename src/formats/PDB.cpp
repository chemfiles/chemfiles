// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <cstdint>

#include <map>
#include <tuple>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Atom.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/Connectivity.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/parse.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/string_view.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/formats/PDB.hpp"
#include "chemfiles/pdb_connectivity.hpp"

using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<PDBFormat>() {
    return FormatInfo("PDB").with_extension(".pdb").description(
        "PDB (RCSB Protein Data Bank) text format"
    );
}

/// Check the number of digits before the decimal separator to be sure than
/// we can represen them. In case of error, use the given `context` in the error
/// message
static void check_values_size(const Vector3D& values, unsigned width, const std::string& context);

// PDB record handled by chemfiles. Any record not in this enum are not yet
// implemented.
enum class Record {
    // Records containing summary data
    HEADER,
    TITLE,
    // Records containing usefull data
    CRYST1,
    ATOM,
    HETATM,
    CONECT,
    // Beggining of model.
    MODEL,
    // End of model.
    ENDMDL,
    // End of chain. May increase atom count
    TER,
    // End of file
    END,
    // Secondary structure
    HELIX,
    SHEET,
    TURN,
    // Ignored records
    IGNORED_,
    // Unknown record type
    UNKNOWN_,
};

// Get the record type for a line.
static Record get_record(string_view line);

void PDBFormat::read_next(Frame& frame) {
    residues_.clear();
    atom_offsets_.clear();

    uint64_t position;
    bool got_end = false;
    while (!got_end && !file_.eof()) {
        auto line = file_.readline();
        auto record = get_record(line);
        std::string name;
        switch (record) {
        case Record::HEADER:
            if (line.size() >= 50) {
                frame.set("classification", trim(line.substr(10, 40)).to_string());
            }
            if (line.size() >= 59) {
                frame.set("deposition_date", trim(line.substr(50, 9)).to_string());
            }
            if (line.size() >= 66) {
                frame.set("pdb_idcode", trim(line.substr(62, 4)).to_string());
            }
            continue;
        case Record::TITLE:
            if (line.size() < 11) {continue;}
            // get previous frame name (from a previous TITLE record) and
            // append to it
            name = frame.get<Property::STRING>("name").value_or("");
            name = name.empty() ? "" : name + " ";
            frame.set("name", name + trim(line.substr(10 , 70)));
            continue;
        case Record::CRYST1:
            read_CRYST1(frame, line);
            continue;
        case Record::ATOM:
            read_ATOM(frame, line, false);
            continue;
        case Record::HETATM:
            read_ATOM(frame, line, true);
            continue;
        case Record::CONECT:
            read_CONECT(frame, line);
            continue;
        case Record::MODEL:
            models_++;
            continue;
        case Record::ENDMDL:
            // Check if the next record is an `END` record
            if (!file_.eof()) {
                position = file_.tellpos();
                line = file_.readline();
                file_.seekpos(position);
                if (get_record(line) == Record::END) {
                    // If this is the case, wait for this next record
                    continue;
                }
            }
            // Else we have read a frame
            got_end = true;
            continue;
        case Record::HELIX:
            read_HELIX(line);
            continue;
        case Record::SHEET:
            read_secondary(line, 21, 32, "extended");
            continue;
        case Record::TURN:
            read_secondary(line, 19, 30, "turn");
            continue;
        case Record::TER:
            if (line.size() >= 12) {
                try {
                    atom_offsets_.push_back(parse<size_t>(line.substr(6, 5)));
                } catch (const Error&) {
                    warning("TER record not numeric: {}", line);
                }
            }
            chain_ended(frame);
            continue;
        case Record::END:
            // We have read a frame!
            got_end = true;
            continue;
        case Record::IGNORED_:
            continue; // Nothing to do
        case Record::UNKNOWN_:
            if (!file_.eof()) {
                warning("Unknown PDB record: {}", line);
            }
            continue;
        }
    }

    if (!got_end) {
        warning("Missing END record in PDB file");
    }

    chain_ended(frame);
    link_standard_residue_bonds(frame);
}

void PDBFormat::read_CRYST1(Frame& frame, string_view line) {
    assert(line.substr(0, 6) == "CRYST1");
    if (line.length() < 54) {
        throw format_error("CRYST1 record '{}' is too small", line);
    }
    try {
        auto a = parse<double>(line.substr(6, 9));
        auto b = parse<double>(line.substr(15, 9));
        auto c = parse<double>(line.substr(24, 9));
        auto alpha = parse<double>(line.substr(33, 7));
        auto beta = parse<double>(line.substr(40, 7));
        auto gamma = parse<double>(line.substr(47, 7));
        auto cell = UnitCell(a, b, c, alpha, beta, gamma);

        frame.set_cell(cell);
    } catch (const Error&) {
        throw format_error("could not read CRYST1 record '{}'", line);
    }

    if (line.length() >= 55) {
        auto space_group = trim(line.substr(55, 10));
        if (space_group != "P 1" && space_group != "P1") {
            warning(
                "space group which is not P1 ({}) ignored in '{}'",
                space_group, file_.path()
            );
        }
    }
}

void PDBFormat::read_HELIX(string_view line) {
    if (line.length() < 33 + 5) {
        warning("HELIX record too short: '{}'", line);
        return;
    }

    auto chain1 = line[19];
    auto chain2 = line[31];
    size_t start = 0;
    size_t end = 0;
    auto inscode1 = line[25];
    auto inscode2 = line[37];

    try {
        start = parse<size_t>(line.substr(21, 4));
        end = parse<size_t>(line.substr(33, 4));
    } catch (const Error&) {
        warning("HELIX record contains invalid numbers: '{}'", line);
        return;
    }

    if (chain1 != chain2) {
        warning("HELIX chain {} and {} are not the same.", chain1, chain2);
        return;
    }

    // Convert the code as a character to its numeric meaning.
    // See http://www.wwpdb.org/documentation/file-format-content/format23/sect5.html
    // for definitions of these numbers
    auto start_info = std::make_tuple(chain1, start, inscode1);
    auto end_info = std::make_tuple(chain2, end, inscode2);

    size_t helix_type = 0;
    try {
        helix_type = parse<size_t>(line.substr(38,2));
    } catch (const Error&) {
        warning("[PDB reader] could not parse helix type");
        return;
    }

    switch (helix_type) {
    case 1: // Treat right and left handed helixes the same.
    case 6:
        secinfo_.emplace_back(std::make_tuple(start_info, end_info, "alpha helix"));
        break;
    case 2:
    case 7:
        secinfo_.emplace_back(std::make_tuple(start_info, end_info, "omega helix"));
        break;
    case 3:
        secinfo_.emplace_back(std::make_tuple(start_info, end_info, "pi helix"));
        break;
    case 4:
    case 8:
        secinfo_.emplace_back(std::make_tuple(start_info, end_info, "gamma helix"));
        break;
    case 5:
        secinfo_.emplace_back(std::make_tuple(start_info, end_info, "3-10 helix"));
        break;
    default:
        break;
    }
}

void PDBFormat::read_secondary(string_view line, size_t i1, size_t i2, string_view record) {
    if (line.length() < i2 + 6) {
        warning("Secondary structure record too short: '{}'", line);
        return;
    }

    auto chain1 = line[i1];
    auto chain2 = line[i2];

    if (chain1 != chain2) {
        warning("{} chain {} and {} are not the same.", record, chain1, chain2);
        return;
    }

    size_t resid1 = 0;
    size_t resid2 = 0;
    try {
        resid1 = parse<size_t>(line.substr(i1 + 1, 4));
        resid2 = parse<size_t>(line.substr(i2 + 1, 4));
    } catch (const Error&) {
        warning(
            "error parsing line: '{}', check {} and {}",
            line, line.substr(i1 + 1, 4), line.substr(i2 + 1, 4)
        );
        return;
    }

    auto inscode1 = line[i1 + 5];
    auto inscode2 = line[i2 + 5];

    auto start = std::make_tuple(chain1, resid1, inscode1);
    auto end = std::make_tuple(chain2, resid2, inscode2);

    secinfo_.emplace_back(std::make_tuple(start, end, "extended"));
}

void PDBFormat::read_ATOM(Frame& frame, string_view line,
    bool is_hetatm) {
    assert(line.substr(0, 6) == "ATOM  " || line.substr(0, 6) == "HETATM");

    if (line.length() < 54) {
        throw format_error(
            "{} record is too small: '{}'", line.substr(0, 6), line
        );
    }

    if (atom_offsets_.empty()) {
        try {
            auto initial_offset = parse<long long>(line.substr(6, 5));
            // We need to handle negative numbers ourselves: https://ideone.com/RdINqa
            if (initial_offset <= 0) {
                warning("{} is too small, assuming id is '1'", initial_offset);
                atom_offsets_.push_back(0);
            } else {
                atom_offsets_.push_back(static_cast<size_t>(initial_offset) - 1);
            }
        } catch(const Error&) {
            warning("{} is not a valid atom id, assuming '1'", line.substr(6, 5));
			atom_offsets_.push_back(0);
        }
    }

    Atom atom;
    auto name = line.substr(12, 4);
    if (line.length() >= 78) {
        auto type = line.substr(76, 2);
        // Read both atom name and atom type
        atom = Atom(trim(name).to_string(), trim(type).to_string());
    } else {
        // Read just the atom name and hope for the best.
        Atom(trim(name).to_string());
    }

    auto altloc = line.substr(16, 1);
    if (altloc != " ") {
        atom.set("altloc", altloc.to_string());
    }

    try {
        auto x = parse<double>(line.substr(30, 8));
        auto y = parse<double>(line.substr(38, 8));
        auto z = parse<double>(line.substr(46, 8));

        frame.add_atom(std::move(atom), Vector3D(x, y, z));
    } catch (const Error&) {
        throw format_error("could not read positions in '{}'", line);
    }

    auto atom_id = frame.size() - 1;
    auto insertion_code = line[26];
    try {
        auto resid = parse<size_t>(line.substr(22, 4));
        auto chain = line[21];
        auto complete_residue_id = std::make_tuple(chain,resid,insertion_code);
        if (residues_.find(complete_residue_id) == residues_.end()) {
            auto resname = trim(line.substr(17, 3));
            Residue residue(resname.to_string(), resid);
            residue.add_atom(atom_id);

            if (insertion_code != ' ') {
                residue.set("insertion_code", line.substr(26, 1).to_string());
            }

            // Set whether or not the residue is standardized
            residue.set("is_standard_pdb", !is_hetatm);
            // This is saved as a string (instead of a number) on purpose
            // to match MMTF format
            residue.set("chainid", line.substr(21, 1).to_string());
            // PDB format makes no distinction between chainid and chainname
            residue.set("chainname", line.substr(21, 1).to_string());
            residues_.insert({complete_residue_id, residue});
        } else {
            // Just add this atom to the residue
            residues_.at(complete_residue_id).add_atom(atom_id);
        }
    } catch (const Error&) {
        // No residue information
    }
}

void PDBFormat::read_CONECT(Frame& frame, string_view line) {
    assert(line.substr(0, 6) == "CONECT");
    auto line_length = trim(line).length();

    // Helper lambdas
    auto add_bond = [&frame, &line](size_t i, size_t j) {
        if (i >= frame.size() || j >= frame.size()) {
            warning("Bad atomic numbers in CONECT record, ignored. ({})", line);
            return;
        }
        frame.add_bond(i, j);
    };

    auto read_index = [&line,this](size_t initial) -> size_t {
        try {
            auto pdb_atom_id = parse<size_t>(line.substr(initial, 5));
            auto lower = std::lower_bound(atom_offsets_.begin(),
                                          atom_offsets_.end(), pdb_atom_id);
            pdb_atom_id -= static_cast<size_t>(lower - atom_offsets_.begin());
            pdb_atom_id -= atom_offsets_.front();
            return pdb_atom_id;
        } catch (const Error&) {
            throw format_error("could not read atomic number in '{}'", line);
        }
    };

    auto i = read_index(6);

    if (line_length > 11) {
        auto j = read_index(11);
        add_bond(i, j);
    } else {
        return;
    }

    if (line_length > 16) {
        auto j = read_index(16);
        add_bond(i, j);
    } else {
        return;
    }

    if (line_length > 21) {
        auto j = read_index(21);
        add_bond(i, j);
    } else {
        return;
    }

    if (line_length > 26) {
        auto j = read_index(26);
        add_bond(i, j);
    } else {
        return;
    }
}

void PDBFormat::chain_ended(Frame& frame) {
    for (const auto& secinfo: secinfo_) {
        auto start = residues_.lower_bound(std::get<0>(secinfo));
        auto end = residues_.upper_bound(std::get<1>(secinfo));
        for (auto residue = start; residue != end; ++residue) {
            residue->second.set("secondary_structure", std::get<2>(secinfo));
        }
    }

    for (const auto& residue: residues_) {
        frame.add_residue(residue.second);
    }

    // This is a 'hack' to allow for badly formatted PDB files which restart
    // the residue ID after a TER residue in cases where they should not.
    // IE a metal Ion given the chain ID of A and residue ID of 1 even though
    // this residue already exists.
    residues_.clear();
}

void PDBFormat::link_standard_residue_bonds(Frame& frame) {
    bool link_previous_peptide = false;
    bool link_previous_nucleic = false;
    uint64_t previous_residue_id = 0;
    size_t previous_carboxylic_id = 0;

    for (const auto& residue: frame.topology().residues()) {
        auto residue_table = PDBConnectivity::find(residue.name());
        if (!residue_table) {
            continue;
        }

        std::map<std::string, size_t> atom_name_to_index;
        for (size_t atom : residue) {
            atom_name_to_index[frame[atom].name()] =  atom;
        }

        const auto& amide_nitrogen = atom_name_to_index.find("N");
        const auto& amide_carbon = atom_name_to_index.find("C");

        if (!residue.id()) {
            warning("got a residues without id in PDB format. This should not happen");
            continue;
        }

        auto resid = *residue.id();
        if (link_previous_peptide &&
            amide_nitrogen != atom_name_to_index.end() &&
            resid == previous_residue_id + 1 )
        {
            link_previous_peptide = false;
            frame.add_bond(previous_carboxylic_id, amide_nitrogen->second);
        }

        if (amide_carbon != atom_name_to_index.end() ) {
            link_previous_peptide = true;
            previous_carboxylic_id = amide_carbon->second;
            previous_residue_id = resid;
        }

        const auto& three_prime_oxygen = atom_name_to_index.find("O3'");
        const auto& five_prime_phospho = atom_name_to_index.find("P");

        if (link_previous_nucleic &&
            five_prime_phospho != atom_name_to_index.end() &&
            resid == previous_residue_id + 1 )
        {
            link_previous_nucleic = false;
            frame.add_bond(previous_carboxylic_id, three_prime_oxygen->second);
        }

        if (three_prime_oxygen != atom_name_to_index.end() ) {
            link_previous_nucleic = true;
            previous_carboxylic_id = three_prime_oxygen->second;
            previous_residue_id = resid;
        }

        // A special case missed by the standards committee????
        if (atom_name_to_index.count("HO5'") != 0) {
            frame.add_bond(atom_name_to_index["HO5'"], atom_name_to_index["O5'"]);
        }

        for (const auto& link: *residue_table) {
            const auto& first_atom = atom_name_to_index.find(link.first);
            const auto& second_atom = atom_name_to_index.find(link.second);

            if (first_atom == atom_name_to_index.end()) {
                const auto& first_name = link.first.string();
                if (first_name[0] != 'H' && first_name != "OXT" &&
                    first_name[0] != 'P' && first_name.substr(0, 2) != "OP" ) {
                    warning("{}_{} does not contain {}", residue.name(), resid, first_name);
                }
                continue;
            }

            if (second_atom == atom_name_to_index.end()) {
                const auto& second_name = link.second.string();
                if (second_name[0] != 'H' && second_name != "OXT" &&
                    second_name[0] != 'P' && second_name.substr(0, 2) != "OP" ) {
                    warning("{}_{} does not contain {}", residue.name(), resid, second_name);
                }
                continue;
            }

            frame.add_bond(first_atom->second, second_atom->second);
        }
    }
}

Record get_record(string_view line) {
    auto rec = line.substr(0, 6);
    if(rec == "ENDMDL") {
        return Record::ENDMDL;
    } else if(rec.substr(0, 3) == "END") {
        // Handle missing whitespace in END record
        return Record::END;
    } else if (rec == "CRYST1") {
        return Record::CRYST1;
    } else if (rec == "ATOM  ") {
        return Record::ATOM;
    } else if (rec == "HETATM") {
        return Record::HETATM;
    } else if (rec == "CONECT") {
        return Record::CONECT;
    } else if (rec.substr(0, 5) == "MODEL") {
        return Record::MODEL;
    } else if (rec.substr(0, 3) == "TER") {
        return Record::TER;
    } else if (rec == "HELIX ") {
        return Record::HELIX;
    } else if (rec == "SHEET ") {
        return Record::SHEET;
    } else if (rec == "TURN  ") {
        return Record::TURN;
    } else if (rec == "HEADER") { // These appear the least, so check last
        return Record::HEADER;
    } else if (rec == "TITLE ") {
        return Record::TITLE;
    } else if (rec == "REMARK" || rec == "MASTER" || rec == "AUTHOR" ||
               rec == "CAVEAT" || rec == "COMPND" || rec == "EXPDTA" ||
               rec == "KEYWDS" || rec == "OBSLTE" || rec == "SOURCE" ||
               rec == "SPLIT " || rec == "SPRSDE" || rec == "JRNL  " ||
               rec == "SEQRES" || rec == "HET   " || rec == "REVDAT" ||
               rec == "SCALE1" || rec == "SCALE2" || rec == "SCALE3" ||
               rec == "ORIGX1" || rec == "ORIGX2" || rec == "ORIGX3" ||
               rec == "SCALE1" || rec == "SCALE2" || rec == "SCALE3" ||
               rec == "ANISOU" || rec == "SITE  " || rec == "FORMUL" ||
               rec == "DBREF " || rec == "HETNAM" || rec == "HETSYN" ||
               rec == "SSBOND" || rec == "LINK  " || rec == "SEQADV" ||
               rec == "MODRES" || rec == "SEQRES" || rec == "CISPEP") {
        return Record::IGNORED_;
    } else if (trim(line).empty()) {
        return Record::IGNORED_;
    } else {
        return Record::UNKNOWN_;
    }
}

static std::string to_pdb_index(uint64_t i) {
    auto id = i + 1;

    if (id >= 100000) {
        if (id == 100000) {
            // Only warn once for this
            warning("Too many atoms for PDB format, removing atomic id bigger than 100000");
        }
        return "*****";
    } else {
        return std::to_string(i + 1);
    }
}

void PDBFormat::write_next(const Frame& frame) {
    written_ = true;
    file_.print("MODEL {:>4}\n", models_ + 1);

    auto& cell = frame.cell();
    check_values_size(Vector3D(cell.a(), cell.b(), cell.c()), 9, "cell lengths");
    file_.print(
        // Do not try to guess the space group and the z value, just use the
        // default one.
        "CRYST1{:9.3f}{:9.3f}{:9.3f}{:7.2f}{:7.2f}{:7.2f} P 1           1\n",
        cell.a(), cell.b(), cell.c(), cell.alpha(), cell.beta(), cell.gamma());

    // Only use numbers bigger than the biggest residue id as "resSeq" for
    // atoms without associated residue.
    uint64_t max_resid = 0;
    for (const auto& residue: frame.topology().residues()) {
        auto resid = residue.id();
        if (resid && resid.value() > max_resid) {
            max_resid = resid.value();
        }
    }

    auto& positions = frame.positions();
    for (size_t i = 0; i < frame.size(); i++) {

        auto altloc = frame[i].get<Property::STRING>("altloc").value_or(" ");
        if (altloc.length() > 1) {
            warning(
                "altloc '{}' is too long for PDB format, it will be truncated",
                altloc
            );
            altloc = altloc[0];
        }

        std::string atom_hetatm = "HETATM";
        std::string resname;
        std::string resid;
        std::string chainid;
        std::string inscode;
        auto residue = frame.topology().residue_for_atom(i);
        if (residue) {
            resname = residue->name();
            if (residue->get<Property::BOOL>("is_standard_pdb").value_or(false)) {
                // only use ATOM if the residue is standardized
                atom_hetatm = "ATOM  ";
            }

            if (resname.length() > 3) {
                warning(
                    "residue '{}' has a name too long for PDB format, it will be truncated",
                    resname
                );
                resname = resname.substr(0, 3);
            }

            if (residue->id()) {
                auto value = residue->id().value();
                if (value > 9999) {
                    warning("too many residues for PDB format, removing residue id {}", value);
                    resid = "  -1";
                } else {
                    resid = std::to_string(residue->id().value());
                }
            } else {
                resid = "  -1";
            }

            if (residue->get("chainid") &&
                residue->get("chainid")->kind() == Property::STRING) {
                chainid = residue->get("chainid")->as_string();
                if (chainid.length() > 1) {
                    warning(
                        "residue '{}' has a chain id too long for PDB format, it will be truncated",
                        chainid
                    );
                    chainid = chainid[0];
                }
            } else {
                chainid = "X";
            }

            if (residue->get("insertion_code") &&
                residue->get("insertion_code")->kind() == Property::STRING) {
                inscode = residue->get("insertion_code")->as_string();
                if (inscode.length() > 1) {
                    warning(
                        "residue '{}' has an insertion code too long for PDB format, it will be truncated",
                        inscode
                    );
                    inscode = inscode[0];
                }
            } else {
                inscode = " ";
            }
        } else {
            resname = "XXX";
            chainid = "X";
            auto value = max_resid++;
            if (value < 9999) {
                resid = to_pdb_index(value);
            } else {
                resid = "  -1";
            }
        }

        assert(resname.length() <= 3);
        auto& pos = positions[i];
        check_values_size(pos, 8, "atomic position");

        file_.print(
            "{: <6}{: >5} {: <4s}{:1}{:3} {:1}{: >4s}{:1}   {:8.3f}{:8.3f}{:8.3f}{:6.2f}{:6.2f}          {: >2s}\n",
            atom_hetatm, to_pdb_index(i), frame[i].name(), altloc, resname, chainid, resid, inscode, pos[0], pos[1], pos[2], 1.0, 0.0, frame[i].type()
        );
    }

    auto connect = std::vector<std::vector<size_t>>(frame.size());
    for (auto& bond : frame.topology().bonds()) {
        if (bond[0] > 99999 || bond[1] > 99999) {
            warning("atomic index is too big for CONNECT, removing the bond between {} and {}", bond[0], bond[1]);
            continue;
        }
        connect[bond[0]].push_back(bond[1]);
        connect[bond[1]].push_back(bond[0]);
    }

    for (size_t i = 0; i < frame.size(); i++) {
        auto connections = connect[i].size();
        auto lines = connections / 4 + 1;
        if (connections == 0) {continue;}

        for (size_t conect_line = 0; conect_line < lines; conect_line++) {
            file_.print("CONECT{: >5}", to_pdb_index(i));
            auto last = std::min(connections, 4 * (conect_line + 1));
            for (size_t j = 4 * conect_line; j < last; j++) {
                file_.print("{: >5}", to_pdb_index(connect[i][j]));
            }
            file_.print("\n");
        }
    }

    file_.print("ENDMDL\n");

    models_++;
}

void check_values_size(const Vector3D& values, unsigned width, const std::string& context) {
    double max_pos = std::pow(10.0, width) - 1;
    double max_neg = -std::pow(10.0, width - 1) + 1;
    if (values[0] > max_pos || values[1] > max_pos || values[2] > max_pos ||
        values[0] < max_neg || values[1] < max_neg || values[2] < max_neg) {
        throw format_error(
            "value in {} is too big for representation in PDB format", context
        );
    }
}

PDBFormat::~PDBFormat() noexcept {
    if (written_) {
      file_.print("END\n");
    }
}

optional<uint64_t> PDBFormat::forward() {
    auto position = file_.tellpos();

    while (!file_.eof()) {
        auto line = file_.readline();

        if (line.substr(0, 6) == "ENDMDL") {
            auto save = file_.tellpos();
            auto next = file_.readline();
            file_.seekpos(save);

            if (next.substr(0, 3) == "END") {
                // We found another record starting by END in the next line,
                // we skip this one and wait for the next one
                continue;
            }
        }

        if (line.substr(0, 3) == "END") {
            return position;
        }
    }

    // Handle missing END record in the file
    if (position == 0) {
        return position;
    } else {
        return nullopt;
    }
}
