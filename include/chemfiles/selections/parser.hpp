/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHFL_SELECTION_PARSER_HPP
#define CHFL_SELECTION_PARSER_HPP
#include "chemfiles/Error.hpp"
#include "chemfiles/Selections.hpp"
#include "chemfiles/selections/lexer.hpp"

namespace chemfiles {
namespace selections {

//! Parse and create an AST from a stream of tokens
//!
//! @throws ParserError if the
Ast parse(std::vector<Token> tstream);

//! @class Expr parser.hpp
//! @brief Abstract base class for expressions in the selection AST
class Expr {
public:
    //! Pretty-printing of this expression. The expression should use a shift
    //! of `delta` spaces in case of multilines output.
    virtual std::string print(unsigned delta = 0) const = 0;
    virtual ~Expr() = default;

    Expr() = default;
    Expr(const Expr&) = delete;
    Expr& operator=(const Expr&) = delete;

    Expr(Expr&&) = default;
    Expr& operator=(Expr&&) = default;
};

//! Pretty-print an AST to any output stream
std::ostream& operator<<(std::ostream& out, const Ast& expr);

}} // namespace chemfiles && namespace selections

#endif
