/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHFL_SELECTION_LEXER_HPP
#define CHFL_SELECTION_LEXER_HPP
#include <string>
#include <vector>
#include <ostream>
#include <cassert>

namespace chemfiles {
namespace selections {

/*! A token in the selection stream
 *
 * Various tokens are alowed here:
 *
 * 	- binary comparison operators (== != < <= > >=);
 * 	- boolean operators (and not or);
 * 	- numbers, the scientific notation is alowed;
 * 	- identifiers, obeing to the ([a-Z][a-Z_1-9]+) regular expression
 */
class Token {
public:
    //! Available token types
    enum TokenType {
        LPAREN, //! Left parenthesis
        RPAREN, //! Right parenthesis
        EQ,     //! "==" token
        NEQ,    //! "!=" token
        LT,     //! "<" token
        LE,     //! "<=" token
        GT,     //! ">" token
        GE,     //! ">=" token
        NOT,    //! "not" token
        AND,    //! "and" token
        OR,     //! "or" token
        IDENT,  //! Generic identifier
        NUM,    //! Number
    };

    //! Basic copy and move constructors
    Token(const Token&) = default;
    Token& operator=(const Token&) = default;
    Token(Token&&) = default;
    Token& operator=(Token&&) = default;

    //! Create an identifier token with `data` name
    //! \post `type()` is IDENT.
    Token(const std::string& data): Token(IDENT, data, 0.0) {}
    //! Create a number token with `data` value
    //! \post `type()` is NUM.
    Token(double data): Token(NUM, "", data) {}
    //! Create a token with type `ttype`.
    //! \pre `ttype` can not be NUM or IDENT.
    //! \post `type()` is `ttype`.
    Token(TokenType ttype): Token(ttype, "", 0.0) {
        assert(ttype != IDENT && ttype != NUM && "Can only use this constructor for token without data");
    }

    //! Get the number value associated with this token.
    //! \pre type() must be `NUM`.
    double number() const {
        assert(type_ == NUM && "Can only get number from NUM token");
        return number_;
    }

    //! Get the string which is at the origin of this token
    std::string str() const;

    //! Get the identifier name associated with this token.
    //! \pre type() must be `IDENT`.
    const std::string& ident() const {
        assert(type_ == IDENT && "Can only get identifiers from IDENT token");
        return ident_;
    }

    //! Check whether this token is a boolean operator, *i.e.* one of `and`, `or` or `not`
    bool is_boolean_op() const {
        return (type_ == AND) || (type_ == OR) || (type_ == NOT);
    }

    //! Check whether this token is a binary comparison operator, *i.e.* one of `==`, `!=`
    //! `<`, `<=`, `>` or `>=`.
    bool is_binary_op() const {
        return (type_ == EQ) || (type_ == NEQ) ||
               (type_ == LT) || (type_ == LE)  ||
               (type_ == GT) || (type_ == GE);
    }

    //! Check whether this token is an operator, either a binary comparison operator or a
    //! boolean operator
    bool is_operator() const {
        return is_binary_op() || is_boolean_op();
    }

    //! Get the precedence of this token. Parentheses have a precedence of 0, operators
    //! are classified by precedence.
    //! \pre This token must be an operator (`is_operator()` is `true`) or a parenthese.
    unsigned precedence() const;

    //! Get the token type of this token
    TokenType type() const {return type_;}
private:
    Token(TokenType ttype, const std::string& s_data, double f_data): type_(ttype), number_(f_data), ident_(s_data) {}
    //! Token type
    TokenType type_;
    //! Value of the number if the token is a TokenType::NUM
    double number_;
    //! Value of the identifier if the token is a TokenType::IDENT
    std::string ident_;
};

//! Convert an `input` string to a stream of tokens
//!
//! @throws LexerError if the input string can not be tokenized
std::vector<Token> tokenize(const std::string& input);

}} // namespace chemfiles && namespace selections

#endif
