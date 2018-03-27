// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <algorithm>
#include <functional>

#include "chemfiles/selections/lexer.hpp"

#include "chemfiles/unreachable.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/utils.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

// This intentionally does not account for other encoding or locale. Selection
// strings should be given in ASCII or UTF-8 encoding.
static bool is_alpha(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static bool is_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' ||
            c == '\f');
}

static bool is_ident_component(char c) {
    return is_alpha(c) || is_digit(c) || c == '_';
}

std::string Token::str() const {
    switch (type_) {
    case Token::END:
        return "<end of selection>";
    case Token::LPAREN:
        return "(";
    case Token::RPAREN:
        return ")";
    case Token::COMMA:
        return ",";
    case Token::VARIABLE:
        return "#" + std::to_string(variable_);
    case Token::EQUAL:
        return "==";
    case Token::NOT_EQUAL:
        return "!=";
    case Token::LESS:
        return "<";
    case Token::LESS_EQUAL:
        return "<=";
    case Token::GREATER:
        return ">";
    case Token::GREATER_EQUAL:
        return ">=";
    case Token::PLUS:
        return "+";
    case Token::MINUS:
        return "-";
    case Token::STAR:
        return "*";
    case Token::SLASH:
        return "/";
    case Token::HAT:
        return "^";
    case Token::NOT:
        return "not";
    case Token::AND:
        return "and";
    case Token::OR:
        return "or";
    case Token::IDENT:
        return ident();
    case Token::NUMBER:
        if (lround(number()) == number()) {
            return std::to_string(lround(number()));
        } else {
            return std::to_string(number());
        }
    }
    unreachable();
}


std::vector<Token> selections::tokenize(const std::string& input) {
    auto tokens = std::vector<Token>();
    for (size_t i=0; i<input.length(); i++) {
        auto c = input[i];
        if (is_space(c)) {
            continue;
        } else if (c == '(') {
            tokens.emplace_back(Token(Token::LPAREN));
            continue;
        } else if (c == ')') {
            tokens.emplace_back(Token(Token::RPAREN));
            continue;
        } else if (c == ',') {
            tokens.emplace_back(Token(Token::COMMA));
            continue;
        } else if (c == '=' && input[i + 1] == '=') {
            i++;
            tokens.emplace_back(Token(Token::EQUAL));
            continue;
        } else if (c == '!' && input[i + 1] == '=') {
            i++;
            tokens.emplace_back(Token(Token::NOT_EQUAL));
            continue;
        } else if (c == '<') {
            if (i + 1 < input.length() && input[i + 1] == '=') {
                i++;
                tokens.emplace_back(Token(Token::LESS_EQUAL));
            } else {
                tokens.emplace_back(Token(Token::LESS));
            }
            continue;
        } else if (c == '>') {
            if (i + 1 < input.length() && input[i + 1] == '=') {
                i++;
                tokens.emplace_back(Token(Token::GREATER_EQUAL));
            } else {
                tokens.emplace_back(Token(Token::GREATER));
            }
            continue;
        } else if (c == '+') {
            tokens.emplace_back(Token(Token::PLUS));
            continue;
        } else if (c == '-') {
            tokens.emplace_back(Token(Token::MINUS));
            continue;
        } else if (c == '*') {
            tokens.emplace_back(Token(Token::STAR));
            continue;
        } else if (c == '/') {
            tokens.emplace_back(Token(Token::SLASH));
            continue;
        } else if (c == '^') {
            tokens.emplace_back(Token(Token::HAT));
            continue;
        } else if (c == '#') {
            // Get the variabel number
            std::string number;
            while (i + 1 < input.length() && is_digit(input[i + 1])) {
                number += input[i + 1];
                i++;
            }
            int data = 0;
            try {
                data = std::stoi(number);
            } catch (const std::exception&) {
                throw selection_error("could not parse number in '{}'", number);
            }
            if (data > UINT8_MAX) {
                throw selection_error("variable index #{} is too big for uint8_t", data);
            }

            tokens.emplace_back(Token::variable(static_cast<uint8_t>(data)));
            continue;
        } else if (is_alpha(c)) {
            // Collect the full identifier
            std::string ident;
            ident += c;
            while (i + 1 < input.length() && is_ident_component(input[i + 1])) {
                ident += input[i + 1];
                i++;
            }
            if (ident == "or") {
                tokens.emplace_back(Token(Token::OR));
                continue;
            } else if (ident == "and") {
                tokens.emplace_back(Token(Token::AND));
                continue;
            } else if (ident == "not") {
                tokens.emplace_back(Token(Token::NOT));
                continue;
            }
            // Default identifier. This will be resolved during parsing phase
            tokens.emplace_back(Token::ident(std::move(ident)));
            continue;
        } else if (is_digit(c)) {
            std::string number;
            number += c;
            while (i + 1 < input.length() && (is_digit(input[i + 1]) || input[i + 1] == '.')) {
                number += input[i + 1];
                i++;
            }

            // Optional float exponent
            if (i + 1 < input.length() && (input[i + 1] == 'e' || input[i + 1] == 'E')) {
                number += input[i + 1];
                i++;

                if (i + 1 < input.length() && (input[i + 1] == '+' || input[i + 1] == '-')) {
                    number += input[i + 1];
                    i++;
                }

                while (i + 1 < input.length() && is_digit(input[i + 1])) {
                    number += input[i + 1];
                    i++;
                }
            }

            // Require a separator between numbers and idents
            if (i + 1 < input.length() && is_ident_component(input[i + 1])) {
                while (i + 1 < input.length() && is_ident_component(input[i + 1])) {
                    number += input[i + 1];
                    i++;
                }
                throw selection_error("identifiers can not start with a digit: '{}'", number);
            }

            try {
                tokens.emplace_back(Token::number(string2double(number)));
                continue;
            } catch (const Error& e) {
                throw SelectionError(e.what());
            }
        } else {
            throw selection_error("unknown char '{}' in '{}'", c, input);
        }
    }
    tokens.push_back(Token(Token::END));
    return tokens;
}
