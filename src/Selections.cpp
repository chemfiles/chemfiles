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

using namespace chemfiles;

Selection::~Selection() = default;
Selection::Selection(Selection&&) = default;
Selection& Selection::operator=(Selection&&) = default;

Selection::Selection(const std::string& selection)
    : selection_(selection), ast_(nullptr) {
    auto tokens = selections::tokenize(selection_);
    ast_ = selections::parse(tokens);
}

std::vector<Bool> Selection::evaluate(const Frame& frame) const {
    return ast_->evaluate(frame);
}
