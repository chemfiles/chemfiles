/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHEMFILES_SELECTION_PARSER_HPP
#define CHEMFILES_SELECTION_PARSER_HPP

#include "chemfiles/Selections.hpp"
#include "chemfiles/selections/lexer.hpp"
#include "chemfiles/bool.hpp"

namespace chemfiles {
namespace selections {

//! Parse and create an AST from a stream of tokens
//!
//! @throws SelectionError if the token stream does not correspond to an AST
CHFL_EXPORT Ast parse(std::vector<Token> tstream);

//! @class Expr parser.hpp
//! @brief Abstract base class for expressions in the selection AST
class Expr {
public:
    //! Pretty-printing of this expression. The expression should use a shift
    //! of `delta` spaces in case of multilines output.
    virtual std::string print(unsigned delta = 0) const = 0;
    //! For each potential match in the `matches` vector this function returns
    //! `true` if the match is valid in the given `frame`.
    virtual std::vector<Bool> evaluate(const Frame& frame, const Matches& matches) const = 0;

    Expr() = default;
    virtual ~Expr() = default;

    Expr(Expr&&) = default;
    Expr& operator=(Expr&&) = default;

    Expr(const Expr&) = delete;
    Expr& operator=(const Expr&) = delete;
};

//! Pretty-print an AST to any output stream
std::ostream& operator<<(std::ostream& out, const Ast& expr);

using token_iterator_t = std::vector<Token>::const_iterator;
//! Dispatch to subexpressions for parsing, from the current value of `begin`
//! This is an internal detail of the parsing algorithm.
Ast dispatch_parsing(token_iterator_t& begin, const token_iterator_t& end);

}} // namespace chemfiles && namespace selections

#endif
