// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <algorithm>

#include "chemfiles/Error.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/selections/lexer.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

// This intentionally does not account for other encoding or locale. Selection
// strings
// should be given in ASCII or UTF-8 encoding.
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

std::string Token::str() const {
    switch (type_) {
    case Token::LPAREN:
        return "(";
    case Token::RPAREN:
        return ")";
    case Token::COMMA:
        return ",";
    case Token::VARIABLE:
        return "#" + std::to_string(variable_);
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
    case Token::NUMBER:
        return std::to_string(number());
    }
    unreachable();
}

unsigned Token::precedence() const {
    switch (type_) {
    case IDENT:
        return 40;
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
    case NUMBER:
    case VARIABLE:
    case COMMA:
        throw SelectionError("Invalid case in Token::precedence");
    }
    unreachable();
}

static std::vector<std::string> split_selection(const std::string& data) {
    std::string token;
    std::vector<std::string> tokens;
    for (auto c : data) {
        if (c == '(' || c == ')' || c == '#' || c == ',') {
            // Handle some tokens that may not be separated from the others
            // tokens by spaces, by splitting them manually.
            if (token.length() != 0) {
                tokens.emplace_back(token);
            }
            token.clear();
            tokens.push_back(std::string{c});
        } else if (!is_space(c)) {
            token += c;
        } else {
            if (token.length() != 0) {
                tokens.emplace_back(token);
            }
            token.clear();
        }
    }
    // Last token
    if (token.length() != 0) {
        tokens.emplace_back(token);
    }
    return tokens;
}

static bool is_identifier(const std::string& token) {
    if (token.length() == 0 || !is_alpha(token[0])) {
        return false;
    }
    auto it = std::find_if_not(std::begin(token), std::end(token), [](char c) {
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
    auto splited = split_selection(input);
    for (size_t i=0; i<splited.size(); i++) {
        auto& word = splited[i];
        if (word == "(") {
            tokens.emplace_back(Token(Token::LPAREN));
            continue;
        } else if (word == ")") {
            tokens.emplace_back(Token(Token::RPAREN));
            continue;
        } else if (word == ",") {
            tokens.emplace_back(Token(Token::COMMA));
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
        } else if (word == "#") {
            if (i == splited.size() - 1) {
                throw SelectionError("Missing value after '#'");
            }
            // Get the next word and try to parse a number out of it
            word = splited[++i];
            try {
                int data = std::stoi(word);
                if (data > UINT8_MAX) {
                    throw SelectionError(
                        "Variable index is too big: " + std::to_string(data)
                    );
                }
                tokens.emplace_back(Token::variable(static_cast<uint8_t>(data)));
                continue;
            } catch (const std::exception&) {
                throw SelectionError("Could not parse number in: '" + word + "'");
            }
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
            tokens.emplace_back(Token::ident(word));
            continue;
        } else if (is_number(word)) {
            tokens.emplace_back(Token::number(string2double(word)));
            continue;
        } else {
            throw SelectionError("Could not parse '" + word + "' in: '" + input + "'");
        }
    }
    return tokens;
}
