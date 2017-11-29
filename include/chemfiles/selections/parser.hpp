// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_SELECTION_PARSER_HPP
#define CHEMFILES_SELECTION_PARSER_HPP

#include "chemfiles/Selections.hpp"
#include "chemfiles/selections/lexer.hpp"

namespace chemfiles {
namespace selections {

/// Parse and create an AST from a stream of tokens
///
/// @throws SelectionError if the token stream does not correspond to an AST
Ast parse(std::vector<Token> tokens);

/// @class Expr parser.hpp
/// @brief Abstract base class for expressions in the selection AST
class Expr {
public:
    /// Pretty-printing of this expression. The expression should use a shift
    /// of `delta` spaces in case of multilines output.
    virtual std::string print(unsigned delta = 0) const = 0;
    /// Check if the `match` is valid in the given `frame`.
    virtual bool is_match(const Frame& frame, const Match& match) const = 0;

    Expr() = default;
    virtual ~Expr() = default;

    Expr(Expr&&) = default;
    Expr& operator=(Expr&&) = default;

    Expr(const Expr&) = delete;
    Expr& operator=(const Expr&) = delete;
};

using token_iterator_t = std::vector<Token>::const_iterator;
/// Dispatch to subexpressions for parsing, from the current value of `begin`
/// This is an internal detail of the parsing algorithm.
Ast dispatch_parsing(token_iterator_t& begin, const token_iterator_t& end);

}} // namespace chemfiles && namespace selections

#endif
