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
        auto context = splited[0];
        trim(context);
        if (context == "atom" || context == "one") {
            return Context::ATOM;
        } else if (context == "pair" || context == "two") {
            return Context::PAIR;
        } else if (context == "three") {
            return Context::THREE;
        } else if (context == "four") {
            return Context::FOUR;
        } else if (context == "bond") {
            return Context::BOND;
        } else if (context == "angle") {
            return Context::ANGLE;
        } else if (context == "dihedral") {
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
    assert(context_ == Context::ATOM);
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
}

Matches Selection::evaluate(const Frame& frame) const {
    auto matches = generate_matches(frame);

    auto valid = ast_->evaluate(frame, matches);
    auto n_valid = std::count(valid.begin(), valid.end(), true);

    auto res = Matches();
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

Matches Selection::generate_matches(const Frame& frame) const {
    assert(size() == 1);
    auto natoms = frame.natoms();
    auto res = Matches(natoms);
    for (size_t i=0; i<natoms; i++) {
        res[i] = Match(i);
    }
    return res;
}
