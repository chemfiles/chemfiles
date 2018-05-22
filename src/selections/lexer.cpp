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
        return "#" + std::to_string(variable_ + 1);
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
    case Token::RAW_IDENT:
        return '"' + ident() + '"';
    case Token::NUMBER:
        if (lround(number()) == number()) {
            return std::to_string(lround(number()));
        } else {
            return std::to_string(number());
        }
    }
    unreachable();
}

std::vector<Token> Tokenizer::tokenize() {
    auto tokens = std::vector<Token>();
    while (!finished()) {
        if (match(is_space)) {
            continue;
        } else if (match('(')) {
            tokens.emplace_back(Token(Token::LPAREN));
            continue;
        } else if (match(')')) {
            tokens.emplace_back(Token(Token::RPAREN));
            continue;
        } else if (match(',')) {
            tokens.emplace_back(Token(Token::COMMA));
            continue;
        } else if (match('=') && match('=')) {
            tokens.emplace_back(Token(Token::EQUAL));
            continue;
        } else if (match('!') && match('=')) {
            tokens.emplace_back(Token(Token::NOT_EQUAL));
            continue;
        } else if (match('<')) {
            if (match('=')) {
                tokens.emplace_back(Token(Token::LESS_EQUAL));
            } else {
                tokens.emplace_back(Token(Token::LESS));
            }
            continue;
        } else if (match('>')) {
            if (match('=')) {
                tokens.emplace_back(Token(Token::GREATER_EQUAL));
            } else {
                tokens.emplace_back(Token(Token::GREATER));
            }
            continue;
        } else if (match('+')) {
            tokens.emplace_back(Token(Token::PLUS));
            continue;
        } else if (match('-')) {
            tokens.emplace_back(Token(Token::MINUS));
            continue;
        } else if (match('*')) {
            tokens.emplace_back(Token(Token::STAR));
            continue;
        } else if (match('/')) {
            tokens.emplace_back(Token(Token::SLASH));
            continue;
        } else if (match('^')) {
            tokens.emplace_back(Token(Token::HAT));
            continue;
        } else if (match('#')) {
            tokens.emplace_back(variable());
            continue;
        } else if (match('"')) {
            tokens.emplace_back(raw_ident());
            continue;
        } else if (check(is_alpha)) {
            tokens.emplace_back(ident());
            continue;
        } else if (check(is_digit)) {
            tokens.emplace_back(number());
            continue;
        } else {
            throw selection_error("unknown char '{}' in '{}'", peek(), input_);
        }
    }
    tokens.push_back(Token(Token::END));
    return tokens;
}

Token Tokenizer::variable() {
    size_t start = current_;
    size_t count = 0;
    while (!finished()) {
        if (match(is_digit)) {
            count += 1;
        } else {
            break;
        }
    }
    int data = 0;
    auto number = input_.substr(start, count);
    try {
        data = std::stoi(number);
    } catch (const std::exception&) {
        throw selection_error("could not parse number in '{}'", number);
    }
    if (data > UINT8_MAX) {
        throw selection_error("variable index #{} is too big for uint8_t", data);
    } else if (data == 0) {
        throw selection_error("invalid variable index #0");
    }
    return Token::variable(static_cast<uint8_t>(data - 1));
}

Token Tokenizer::raw_ident() {
    // assert(previous() == '"');

    size_t start = current_;
    size_t count = 0;
    bool got_quote = false;

    while (!finished()) {
        if (match('"')) {
            got_quote = true;
            break;
        } else {
            advance();
            count += 1;
        }
    }

    if (!got_quote) {
        throw selection_error("missing \" after '{}'", input_.substr(start, count));
    }

    return Token::raw_ident(input_.substr(start, count));
}

Token Tokenizer::ident() {
    size_t start = current_;
    size_t count = 0;
    while (!finished()) {
        if (match(is_ident_component)) {
            count += 1;
        } else {
            break;
        }
    }

    auto ident = input_.substr(start, count);
    if (ident == "or") {
        return Token(Token::OR);
    } else if (ident == "and") {
        return Token(Token::AND);
    } else if (ident == "not") {
        return Token(Token::NOT);
    }
    // Default identifier. This will be resolved during parsing phase
    return Token::ident(std::move(ident));
}

Token Tokenizer::number() {
    size_t start = current_;
    size_t count = 0;
    while (!finished()) {
        if (match(is_digit) || match('.')) {
            count += 1;
        } else {
            break;
        }
    }

    // Optional float exponent
    if (match('e') || match('E')) {
        count += 1;
        if (match('+') || match('-')) {
            count += 1;
        }
        while (!finished()) {
            if (match(is_digit)) {
                count += 1;
            } else {
                break;
            }
        }
    }

    // Require a separator between numbers and idents
    if (match(is_ident_component)) {
        count += 1;
        while (!finished()) {
            if (match(is_ident_component)) {
                count += 1;
            } else {
                break;
            }
        }
        throw selection_error("identifiers can not start with a digit: '{}'", input_.substr(start, count));
    }

    double value = 0;
    try {
        value = string2double(input_.substr(start, count));
    } catch (const Error& e) {
        throw SelectionError(e.what());
    }
    return Token::number(value);
}
