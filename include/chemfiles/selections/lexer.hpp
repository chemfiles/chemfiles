/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHEMFILES_SELECTION_LEXER_HPP
#define CHEMFILES_SELECTION_LEXER_HPP

#include <string>
#include <vector>
#include <cassert>
#include <chemfiles/exports.hpp>

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
class CHFL_EXPORT Token {
public:
    //! Available token types
    enum Type {
        LPAREN,    //! Left parenthesis
        RPAREN,    //! Right parenthesis
        COMMA,     //! Comma
        EQ,        //! "==" token
        NEQ,       //! "!=" token
        LT,        //! "<" token
        LE,        //! "<=" token
        GT,        //! ">" token
        GE,        //! ">=" token
        NOT,       //! "not" token
        AND,       //! "and" token
        OR,        //! "or" token
        IDENT,     //! Generic identifier
        NUMBER,    //! Number
        VARIABLE,  //! "$(\d)" token
    };

    //! Basic copy and move constructors
    Token(const Token&) = default;
    Token& operator=(const Token&) = default;
    Token(Token&&) = default;
    Token& operator=(Token&&) = default;

    //! Create an identifier token with `data` name
    static Token ident(const std::string& data) {
        return Token(IDENT, data, 0.0, 0);
    }

    //! Create a number token with `data` value
    static Token number(double data) {
        return Token(NUMBER, "", data, 0);
    }

    //! Create a variable token with `data` value
    static Token variable(uint8_t variable) {
        return Token(VARIABLE, "", 0.0, variable);
    }

    //! Create a token with type `ttype`.
    //! \pre `ttype` can not be NUM or IDENT.
    //! \post `type()` is `ttype`.
    Token(Type ttype): Token(ttype, "", 0.0, 0) {
        assert(
            ttype != IDENT && ttype != NUMBER && ttype != VARIABLE &&
            "Can only use this constructor for token without associated data"
        );
    }

    //! Get the string which is at the origin of this token
    std::string str() const;

    //! Get the number value associated with this token.
    //! \pre type() must be `NUMBER`.
    double number() const {
        assert(type_ == NUMBER && "Can only get number from NUM token");
        return number_;
    }

    //! Get the identifier name associated with this token.
    //! \pre type() must be `IDENT`.
    const std::string& ident() const {
        assert(type_ == IDENT && "Can only get identifiers from IDENT token");
        return ident_;
    }

    //! Get the variable associated with this token.
    //! \pre type() must be `VARIABLE`.
    uint8_t variable() const {
        assert(type_ == VARIABLE && "Can only get variable from VARIABLE token");
        return variable_;
    }

    //! Check whether this token is a boolean operator, *i.e.* one of `and`,
    //! `or` or `not`
    bool is_boolean_op() const {
        return (type() == AND || type() == OR || type() == NOT);
    }

    //! Check whether this token is a binary comparison operator, *i.e.* one
    //! of `==`, `!=`, `<`, `<=`, `>` or `>=`.
    bool is_binary_op() const {
        return (type() == EQ || type() == NEQ ||
                type() == LT || type() == LE  ||
                type() == GT || type() == GE);
    }

    //! Check whether this token is an operator, either a binary comparison operator or a
    //! boolean operator
    bool is_operator() const {
        return is_binary_op() || is_boolean_op();
    }

    //! Check whether this token is an identifier
    bool is_ident() const {
        return type_ == IDENT;
    }

    //! Check whether this token is a number
    bool is_number() const {
        return type_ == NUMBER;
    }

    //! Check whether this token is a variable
    bool is_variable() const {
        return type_ == VARIABLE;
    }

    //! Get the precedence of this token. Parentheses have a precedence of 0, operators
    //! are classified by precedence.
    //! \pre This token must be an operator (`is_operator()` is `true`) or a parenthese.
    unsigned precedence() const;

    //! Get the token type of this token
    Type type() const {return type_;}
private:
    Token(Type type, const std::string& ident, double number, uint8_t variable)
        : type_(type), number_(number), ident_(ident), variable_(variable) {}
    //! Token type
    Type type_;
    //! Value of the number if the token is a NUMBER
    double number_;
    //! Value of the identifier if the token is an IDENT
    std::string ident_;
    //! Value of the variable if the token is a VARIABLE
    uint8_t variable_;
};

//! Convert an `input` string to a stream of tokens
//!
//! @throws SelectionError if the input string can not be tokenized
CHFL_EXPORT std::vector<Token> tokenize(const std::string& input);

}} // namespace chemfiles && namespace selections

#endif
