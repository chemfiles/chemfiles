// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_SELECTION_LEXER_HPP
#define CHEMFILES_SELECTION_LEXER_HPP

#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <chemfiles/exports.hpp>

namespace chemfiles {
namespace selections {

/// A token in the selection stream
///
/// Various tokens are alowed here:
///
/// - binary comparison operators (== != < <= > >=);
/// - boolean operators (and not or);
/// - numbers, the scientific notation is alowed;
/// - identifiers, obeing to the ([a-Z][a-Z_1-9]+) regular expression
class Token {
public:
    /// Available token types
    enum Type {
        /// Left parenthesis
        LPAREN,
        /// Right parenthesis
        RPAREN,
        /// Comma
        COMMA,
        /// "==" token
        EQ,
        /// "!=" token
        NEQ,
        /// "<" token
        LT,
        /// "<=" token
        LE,
        /// ">" token
        GT,
        /// ">=" token
        GE,
        /// "not" token
        NOT,
        /// "and" token
        AND,
        /// "or" token
        OR,
        /// Generic identifier
        IDENT,
        /// Number
        NUMBER,
        /// "#(\d+)" token
        VARIABLE,
    };

    ~Token() = default;
    Token(const Token&) = default;
    Token& operator=(const Token&) = default;
    Token(Token&&) = default;
    Token& operator=(Token&&) = default;

    /// Create an identifier token with `data` name
    static Token ident(std::string data) {
        return Token(IDENT, std::move(data), 0.0, 0);
    }

    /// Create a number token with `data` value
    static Token number(double data) {
        std::stringstream sstream;
        sstream << data;
        return Token(NUMBER, sstream.str(), data, 0);
    }

    /// Create a variable token with `data` value
    static Token variable(uint8_t variable) {
        return Token(VARIABLE, "", 0.0, variable);
    }

    /// Create a token with type `ttype`.
    /// \pre `ttype` can not be NUM or IDENT.
    /// \post `type()` is `ttype`.
    Token(Type ttype): Token(ttype, "", 0.0, 0) {
        assert(
            ttype != IDENT && ttype != NUMBER && ttype != VARIABLE &&
            "Can only use this constructor for token without associated data"
        );
    }

    /// Get the string which is at the origin of this token
    std::string str() const;

    /// Get the number value associated with this token.
    /// \pre type() must be `NUMBER`.
    double number() const {
        assert(type_ == NUMBER && "Can only get number from NUM token");
        return number_;
    }

    /// Get the identifier name associated with this token.
    /// \pre type() must be `IDENT` or `NUMBER`.
    const std::string& ident() const {
        assert(
            (type_ == IDENT || type_ == NUMBER) &&
            "Can only get identifiers from IDENT or NUMBER token"
        );
        return ident_;
    }

    /// Get the variable associated with this token.
    /// \pre type() must be `VARIABLE`.
    unsigned variable() const {
        assert(type_ == VARIABLE && "Can only get variable from VARIABLE token");
        return variable_;
    }

    /// Check whether this token is a boolean operator,///i.e.* one of `and`,
    /// `or` or `not`
    bool is_boolean_op() const {
        return (type() == AND || type() == OR || type() == NOT);
    }

    /// Check whether this token is a binary comparison operator,///i.e.* one
    /// of `==`, `!=`, `<`, `<=`, `>` or `>=`.
    bool is_binary_op() const {
        return (type() == EQ || type() == NEQ ||
                type() == LT || type() == LE  ||
                type() == GT || type() == GE);
    }

    /// Check whether this token is an operator, either a binary comparison operator or a
    /// boolean operator
    bool is_operator() const {
        return is_binary_op() || is_boolean_op();
    }

    /// Check whether this token is an identifier
    bool is_ident() const {
        return (type_ == IDENT || type_ == NUMBER);
    }

    /// Check whether this token is a number
    bool is_number() const {
        return type_ == NUMBER;
    }

    /// Check whether this token is a variable
    bool is_variable() const {
        return type_ == VARIABLE;
    }

    /// Get the precedence of this token. Parentheses have a precedence of 0, operators
    /// are classified by precedence.
    /// \pre This token must be an operator (`is_operator()` is `true`) or a parenthese.
    unsigned precedence() const;

    /// Get the token type of this token
    Type type() const {return type_;}
private:
    Token(Type type, std::string ident, double number, uint8_t variable)
        : type_(type), number_(number), ident_(std::move(ident)), variable_(variable) {}
    /// Token type
    Type type_;
    /// Value of the number if the token is a NUMBER
    double number_;
    /// Value of the identifier if the token is an IDENT
    std::string ident_;
    /// Value of the variable if the token is a VARIABLE
    unsigned variable_;
};

/// Convert an `input` string to a stream of tokens
///
/// @throws SelectionError if the input string can not be tokenized
std::vector<Token> tokenize(const std::string& input);

}} // namespace chemfiles && namespace selections

#endif
