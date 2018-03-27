// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_SELECTION_LEXER_HPP
#define CHEMFILES_SELECTION_LEXER_HPP

#include <string>
#include <vector>

#include <chemfiles/exports.hpp>
#include <chemfiles/Error.hpp>

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
        EQUAL,
        /// "!=" token
        NOT_EQUAL,
        /// "<" token
        LESS,
        /// "<=" token
        LESS_EQUAL,
        /// ">" token
        GREATER,
        /// ">=" token
        GREATER_EQUAL,
        /// "+" token
        PLUS,
        /// "-" token
        MINUS,
        /// "*" token
        STAR,
        /// "/" token
        SLASH,
        /// "^" token
        HAT,
        /// "and" token
        AND,
        /// "or" token
        OR,
        /// "not" token
        NOT,
        /// Generic identifier
        IDENT,
        /// Number
        NUMBER,
        /// "#(\d+)" token
        VARIABLE,
        /// End of selection
        END,
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
        return Token(NUMBER, "", data, 0);
    }

    /// Create a variable token with `data` value
    static Token variable(uint8_t variable) {
        return Token(VARIABLE, "", 0.0, variable);
    }

    /// Create a token with the given `type`. The type can not be `NUMBER`,
    /// `IDENT` or `VARIABLE`.
    Token(Type type): Token(type, "", 0.0, 0) {
        if (type == IDENT || type == NUMBER || type == VARIABLE) {
            throw Error("invalid Token constructor called. This is a bug.");
        }
    }

    /// Get the string which is at the origin of this token
    std::string str() const;

    /// Get the number value associated with this token.
    /// The token type must be `NUMBER`.
    double number() const {
        if (type_ != NUMBER) {
            throw Error("can not get a number value out of this token. This is a bug.");
        }
        return number_;
    }

    /// Get the identifier name associated with this token.
    /// The token type must be `IDENT`.
    const std::string& ident() const {
        if (type_ != IDENT) {
            throw Error("can not get an ident value out of this token. This is a bug.");
        }
        return ident_;
    }

    /// Get the variable associated with this token.
    /// `type()` must be `VARIABLE`.
    unsigned variable() const {
        if (type_ != VARIABLE) {
            throw Error("can not get a variable value out of this token. This is a bug.");
        }
        return variable_;
    }

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
