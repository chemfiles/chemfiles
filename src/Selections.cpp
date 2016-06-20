/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */
#include "chemfiles/Selections.hpp"
#include "chemfiles/selections/lexer.hpp"
#include "chemfiles/selections/parser.hpp"

#include <algorithm>
#include <numeric>

#include "chemfiles/Error.hpp"
#include "chemfiles/utils.hpp"
using namespace chemfiles;

//! Extract the context from the `string`, and put the selection string
//! (without the context) in `selection`
static Context get_context(const std::string& string, std::string& selection) {
    auto splited = split(string, ':');
    if (splited.size() == 1) {
        // Default to ATOM
        selection = string;
        return Context::ATOM;
    } else if (splited.size() == 2) {
        selection = splited[1];
        auto context = trim(splited[0]);
        if (context == "atoms" || context == "one") {
            return Context::ATOM;
        } else if (context == "pairs" || context == "two") {
            return Context::PAIR;
        } else if (context == "three") {
            return Context::THREE;
        } else if (context == "four") {
            return Context::FOUR;
        } else if (context == "bonds") {
            return Context::BOND;
        } else if (context == "angles") {
            return Context::ANGLE;
        } else if (context == "dihedrals") {
            return Context::DIHEDRAL;
        } else {
            throw SelectionError(
                "Unknown selection context '" + context + "' in \"" + string + "\""
            );
        }
    } else {
        throw SelectionError(
            "Can not read selection context in \"" + string + "\", too much ':'"
        );
    }
}

Selection::~Selection() = default;
Selection::Selection(Selection&&) = default;
Selection& Selection::operator=(Selection&&) = default;

Selection::Selection(const std::string& selection)
    : selection_(selection), ast_(nullptr) {
    std::string selection_string;
    context_ = get_context(selection, selection_string);
    auto tokens = selections::tokenize(selection_string);
    ast_ = selections::parse(tokens);
}

size_t Selection::size() const {
    switch (context_) {
        case Context::ATOM:
            return 1;
        case Context::PAIR:
        case Context::BOND:
            return 2;
        case Context::THREE:
        case Context::ANGLE:
            return 3;
        case Context::FOUR:
        case Context::DIHEDRAL:
            return 4;
    }
    unreachable();
}

std::vector<Match> Selection::evaluate(const Frame& frame) const {
    auto matches = generate_matches(frame);
    auto valid = ast_->evaluate(frame, matches);
    auto n_valid = std::count(valid.begin(), valid.end(), true);

    auto res = std::vector<Match>();
    res.reserve(static_cast<size_t>(n_valid));
    assert(valid.size() == matches.size());
    for (size_t i=0; i<matches.size(); i++) {
        if (valid[i]) {
            res.push_back(matches[i]);
        }
    }
    return res;
}

std::vector<size_t> Selection::list(const Frame& frame) const {
    if (size() != 1) {
        throw SelectionError("Can not call `list` on a multiple selection");
    }
    auto matches = evaluate(frame);
    auto res = std::vector<size_t>(matches.size());
    for (size_t i=0; i<matches.size(); i++) {
        res[i] = matches[i][0];
    }
    return res;
}

static std::vector<Match> atom_matches(const Frame& frame) {
    auto natoms = frame.natoms();
    auto res = std::vector<Match>(natoms);
    for (size_t i=0; i<natoms; i++) {
        res[i] = Match(i);
    }
    return res;
}

static std::vector<Match> pair_matches(const Frame& frame) {
    auto natoms = frame.natoms();
    size_t npairs = natoms * (natoms - 1) / 2;
    auto res = std::vector<Match>(npairs);
    size_t idx = 0;
    for (size_t i=0; i<natoms; i++) {
        for (size_t j=i+1; j<natoms; j++) {
            res[idx] = Match(i, j);
            idx += 1;
        }
    }
    assert(idx == npairs);
    return res;
}

static std::vector<Match> three_matches(const Frame& frame) {
    auto natoms = frame.natoms();
    size_t nthree = natoms * (natoms - 1) * (natoms - 2) / 6;
    if (natoms >= 2954 && sizeof(size_t) == 4) {
        // This value will cause `nfour` to overflow in 32-bit systems
        throw SelectionError(
            "Can not match 3 atoms when the frame contains more than 2954 atoms"
        );
    }
    auto res = std::vector<Match>(nthree);
    size_t idx = 0;
    for (size_t i=0; i<natoms; i++) {
        for (size_t j=i+1; j<natoms; j++) {
            for (size_t k=j+1; k<natoms; k++) {
                res[idx] = Match(i, j, k);
                idx += 1;
            }
        }
    }
    assert(idx == nthree);
    return res;
}

static std::vector<Match> four_matches(const Frame& frame) {
    auto natoms = frame.natoms();
    if (natoms >= 145056 && sizeof(size_t) == 8) {
        // This value will cause `nfour` to overflow in 64-bit systems
        throw SelectionError(
            "Can not match 4 atoms when the frame contains more than 145056 atoms"
        );
    } else if (natoms >= 568 && sizeof(size_t) == 4) {
        // This value will cause `nfour` to overflow in 32-bit systems
        throw SelectionError(
            "Can not match 4 atoms when the frame contains more than 568 atoms"
        );
    }
    size_t nfour = natoms * (natoms - 1) * (natoms - 2) * (natoms - 3) / 24;
    auto res = std::vector<Match>(nfour);
    size_t idx = 0;
    for (size_t i=0; i<natoms; i++) {
        for (size_t j=i+1; j<natoms; j++) {
            for (size_t k=j+1; k<natoms; k++) {
                for (size_t m=k+1; m<natoms; m++) {
                    res[idx] = Match(i, j, k, m);
                    idx += 1;
                }
            }
        }
    }
    assert(idx == nfour);
    return res;
}

static std::vector<Match> bond_matches(const Frame& frame) {
    auto nbonds = frame.topology().bonds().size();
    auto res = std::vector<Match>(nbonds);
    size_t i = 0;
    for (auto& bond: frame.topology().bonds()) {
        res[i] = Match(bond[0], bond[1]);
        i += 1;
    }
    return res;
}

static std::vector<Match> angle_matches(const Frame& frame) {
    auto nangles = frame.topology().angles().size();
    auto res = std::vector<Match>(nangles);
    size_t i = 0;
    for (auto& angle: frame.topology().angles()) {
        res[i] = Match(angle[0], angle[1], angle[2]);
        i += 1;
    }
    return res;
}

static std::vector<Match> dihedral_matches(const Frame& frame) {
    auto ndihedrals = frame.topology().dihedrals().size();
    auto res = std::vector<Match>(ndihedrals);
    size_t i = 0;
    for (auto& dihedral: frame.topology().dihedrals()) {
        res[i] = Match(dihedral[0], dihedral[1], dihedral[2] , dihedral[3]);
        i += 1;
    }
    return res;
}

std::vector<Match> Selection::generate_matches(const Frame& frame) const {
    switch (context_) {
        case Context::ATOM:
            return atom_matches(frame);
        case Context::PAIR:
            return pair_matches(frame);
        case Context::BOND:
            return bond_matches(frame);
        case Context::THREE:
            return three_matches(frame);
        case Context::ANGLE:
            return angle_matches(frame);
        case Context::FOUR:
            return four_matches(frame);
        case Context::DIHEDRAL:
            return dihedral_matches(frame);
    }
    unreachable();
}
