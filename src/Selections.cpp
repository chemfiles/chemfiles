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

#include "chemfiles/Error.hpp"

#include <algorithm>
using namespace chemfiles;

Selection::~Selection() = default;
Selection::Selection(Selection&&) = default;
Selection& Selection::operator=(Selection&&) = default;

Selection::Selection(const std::string& selection)
    : selection_(selection), ast_(nullptr) {
    auto tokens = selections::tokenize(selection_);
    ast_ = selections::parse(tokens);
}

size_t Selection::size() const {
    return 1;
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
