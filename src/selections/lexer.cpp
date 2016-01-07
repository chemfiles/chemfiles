/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */
#include <algorithm>

#include "chemfiles/Error.hpp"
#include "chemfiles/selections/lexer.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

// This intentionally does not account for other encoding or locale. Selection strings
// should be given in ASCII or UTF-8 encoding.
static bool is_alpha(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static bool is_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f');
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
    switch (token.type()) {
    case Token::LPAREN:
        out << "LPAREN";
        break;
    case Token::RPAREN:
        out << "RPAREN";
        break;
    case Token::EQ:
        out << "EQ";
        break;
    case Token::NEQ:
        out << "NEQ";
        break;
    case Token::LT:
        out << "LT";
        break;
    case Token::LE:
        out << "LE";
        break;
    case Token::GT:
        out << "GT";
        break;
    case Token::GE:
        out << "GE";
        break;
    case Token::NOT:
        out << "NOT";
        break;
    case Token::AND:
        out << "AND";
        break;
    case Token::OR:
        out << "OR";
        break;
    case Token::IDENT:
        out << "IDENT(" << token.ident() << ")";
        break;
    case Token::NUM:
        out << "NUM(" << token.number() << ")";
        break;
    default:
        throw std::runtime_error("Hit the default case in Token::operator<<");
    }
    return out;
}

std::string Token::str() const {
    switch (type()) {
    case Token::LPAREN:
        return "(";
    case Token::RPAREN:
        return ")";
    case Token::EQ:
        return "==";
    case Token::NEQ:
        return "!=";
    case Token::LT:
        return "<";
    case Token::LE:
        return "<=";
    case Token::GT:
        return ">";
    case Token::GE:
        return ">=";
    case Token::NOT:
        return "not";
    case Token::AND:
        return "and";
    case Token::OR:
        return "or";
    case Token::IDENT:
        return ident();
    case Token::NUM:
        return std::to_string(number());
    default:
        throw std::runtime_error("Hit the default case in Token::operator<<");
    }
}

unsigned Token::precedence() const {
    assert(is_operator() || type_ == RPAREN || type_ == LPAREN);
    switch (type_) {
    case LT:
    case LE:
    case GT:
    case GE:
    case EQ:
    case NEQ:
        return 30;
    case NOT:
        return 20;
    case AND:
        return 10;
    case OR:
        return 5;
    case LPAREN:
    case RPAREN:
        return 0;
    default:
        throw std::runtime_error("Hit default case in Token::priority");
    }
}

static std::vector<std::string> split(const std::string& data) {
    std::string token;
    std::vector<std::string> tokens;
    for(auto c: data) {
        if (c == '(' || c == ')') {
            // Handle parenthese token. They may not be separated from the others tokens
            // by spaces, so let split them manually.
            if (token.length()) {
                tokens.emplace_back(token);
            }
            token.clear();
            tokens.push_back(std::string{c});
        } else if (!is_space(c)) {
            token += c;
        } else {
            if (token.length()) {
                tokens.emplace_back(token);
            }
            token.clear();
        }
    }
    // Last token
    if (token.length()) {
        tokens.emplace_back(token);
    }
    return tokens;
}

static bool is_identifier(const std::string& token) {
    if (token.length() == 0 || !is_alpha(token[0])) {
        return false;
    }
    auto it = std::find_if_not(std::begin(token), std::end(token), [](char c){
        return is_alpha(c) || is_digit(c) || c == '_';
    });
    return it == std::end(token);
}

static bool is_number(const std::string& token) {
    auto it = std::find_if_not(std::begin(token), std::end(token), [&](char c) {
        return is_digit(c) || c == '.' || c == 'e' || c == '-' || c == '+';
    });
    return it == std::end(token);
}

std::vector<Token> selections::tokenize(const std::string& input) {
    auto tokens = std::vector<Token>();
    for (auto& word: split(input)) {
        if (word == "(") {
            tokens.emplace_back(Token(Token::LPAREN));
            continue;
        } else if (word == ")") {
            tokens.emplace_back(Token(Token::RPAREN));
            continue;
        } else if (word == "==") {
            tokens.emplace_back(Token(Token::EQ));
            continue;
        } else if (word == "!=") {
            tokens.emplace_back(Token(Token::NEQ));
            continue;
        } else if (word == "<") {
            tokens.emplace_back(Token(Token::LT));
            continue;
        } else if (word == "<=") {
            tokens.emplace_back(Token(Token::LE));
            continue;
        } else if (word == ">") {
            tokens.emplace_back(Token(Token::GT));
            continue;
        } else if (word == ">=") {
            tokens.emplace_back(Token(Token::GE));
            continue;
        } else if (is_identifier(word)) {
            if (word == "or") {
                tokens.emplace_back(Token(Token::OR));
                continue;
            } else if (word == "and") {
                tokens.emplace_back(Token(Token::AND));
                continue;
            } else if (word == "not") {
                tokens.emplace_back(Token(Token::NOT));
                continue;
            }
            // Default identifier. This will be resolved during parsing phase
            tokens.emplace_back(Token(word));
            continue;
        } else if (is_number(word)) {
            try {
                double data = std::stod(word);
                tokens.emplace_back(Token(data));
                continue;
            } catch (const std::exception&) {
                throw LexerError("Could not parse number in: '" + word + "'");
            }
        } else {
            throw LexerError("Could not parse '" + word + "' in selection: '" + input + "'");
        }
    }
    return tokens;
}
