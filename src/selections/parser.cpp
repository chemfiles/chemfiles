// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <algorithm>
#include <stack>
#include <map>

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/unreachable.hpp"
#include "chemfiles/selections/expr.hpp"
#include "chemfiles/selections/parser.hpp"

using namespace chemfiles;
using namespace selections;

struct function_info_t {
    /// Function arity, i.e. number of arguments
    unsigned arity;
    /// Does the fucntion admit a short form
    bool has_short_form;
};

static std::map<std::string, function_info_t> FUNCTIONS = {
    {"name", {1, true}},
    {"type", {1, true}},
    {"resname", {1, true}},
    {"resid", {1, true}},
    {"index", {1, true}},
    {"mass", {1, true}},
    {"x", {1, false}},
    {"y", {1, false}},
    {"z", {1, false}},
    {"vx", {1, false}},
    {"vy", {1, false}},
    {"vz", {1, false}},
};

/// Is this token a function token?
static bool is_function(const Token& token) {
    return token.is_ident() && FUNCTIONS.find(token.ident()) != FUNCTIONS.end();
}

/* Standard shunting-yard algorithm, as described in Wikipedia
 * https://en.wikipedia.org/wiki/Shunting-yard_algorithm
 *
 * This convert infix expressions into an AST-like expression, while checking
 * parentheses.
 * The following input:
 *       name == bar and x <= 56
 * is converted to:
 *       and == bar name <= 56 x
 * which is the AST for
 *             and
 *         /          \
 *        ==          <=
 *       /  \        /  \
 *    name   bar    x    56
 */
static std::vector<Token> shunting_yard(std::vector<Token> tokens) {
    std::stack<Token> operators;
    std::vector<Token> output;
    for (auto token: tokens) {
        if (token.is_number() || token.is_variable()) {
            output.emplace_back(std::move(token));
        } else if (token.is_ident()) {
            if (is_function(token)) {
                operators.emplace(std::move(token));
            } else {
                output.emplace_back(std::move(token));
            }
        } else if (token.type() == Token::COMMA) {
            while (operators.top().type() != Token::LPAREN) {
                output.push_back(operators.top());
                operators.pop();
                if (operators.empty()) {
                    throw selection_error(
                        "mismatched paretheses or additional comma found"
                    );
                }
            }
        } else if (token.is_operator()) {
            while (!operators.empty()) {
                // All the operators are left-associative
                if (token.precedence() <= operators.top().precedence()) {
                    output.push_back(operators.top());
                    operators.pop();
                } else {
                    break;
                }
            }
            operators.emplace(std::move(token));
        } else if (token.type() == Token::LPAREN) {
            operators.emplace(std::move(token));
        } else if (token.type() == Token::RPAREN) {
            while (!operators.empty() && operators.top().type() != Token::LPAREN) {
                output.push_back(operators.top());
                operators.pop();
            }

            if (!operators.empty() && is_function(operators.top())) {
                output.push_back(operators.top());
                operators.pop();
            }

            if (operators.empty() || operators.top().type() != Token::LPAREN) {
                throw selection_error("mismatched parentheses");
            } else {
                operators.pop();
            }
        }
    }
    while (!operators.empty()) {
        if (operators.top().type() == Token::LPAREN ||
            operators.top().type() == Token::RPAREN) {
            throw selection_error("mismatched parentheses");
        } else {
            output.push_back(operators.top());
            operators.pop();
        }
    }
    // AST come out as reverse polish notation, let's reverse it for easier
    // parsing after
    std::reverse(std::begin(output), std::end(output));
    return output;
}

/* Rewrite the token stream to convert short form for the expressions to the
 * long one.
 *
 * Short forms are expressions like `name foo` or `index 3`, which are
 * equivalent to `name == foo` and `index == 3`.
 */
static std::vector<Token> add_missing_equals(const std::vector<Token>& stream) {
    auto out = std::vector<Token>();
    for (auto it = stream.cbegin(); it != stream.cend(); it++) {
        if (is_function(*it) && FUNCTIONS[it->ident()].has_short_form) {
            auto next = it + 1;
            if (next->type() != Token::LPAREN) {
                if (next < stream.cend() && !next->is_operator()) {
                    out.emplace_back(*it);
                    out.emplace_back(Token(Token::EQ));
                    continue;
                }
            } else {
                // Skip the following possible tokens: '(' - '#x' - ')'
                next = it + 4;
                if (next < stream.cend() && !next->is_operator() &&
                    it[1].type() == Token::LPAREN && it[2].is_variable() &&
                    it[3].type() == Token::RPAREN
                    ) {
                    out.emplace_back(it[0]);
                    out.emplace_back(it[1]);
                    out.emplace_back(it[2]);
                    out.emplace_back(it[3]);
                    out.emplace_back(Token(Token::EQ));
                    it += 3;
                    continue;
                }
            }
        }
        out.emplace_back(*it);
    }
    return out;
}

Ast selections::dispatch_parsing(token_iterator_t& begin, const token_iterator_t& end) {
    if (begin->is_boolean_op()) {
        switch (begin->type()) {
        case Token::AND:
            return parse<AndExpr>(begin, end);
        case Token::OR:
            return parse<OrExpr>(begin, end);
        case Token::NOT:
            return parse<NotExpr>(begin, end);
        default:
            unreachable();
        }
    } else if (begin->is_binary_op()) {
        if ((end - begin) < 3 || begin[2].type() != Token::IDENT) {
            throw selection_error("bad binary operation around {}", begin->str());
        }

        auto ident = begin[2].ident();
        if (ident == "type") {
            return parse<TypeExpr>(begin, end);
        } else if (ident == "name") {
            return parse<NameExpr>(begin, end);
        } else if (ident == "index") {
            return parse<IndexExpr>(begin, end);
        } else if (ident == "resname") {
            return parse<ResnameExpr>(begin, end);
        } else if (ident == "resid") {
            return parse<ResidExpr>(begin, end);
        } else if (ident == "mass") {
            return parse<MassExpr>(begin, end);
        } else if (ident == "x" || ident == "y" || ident == "z") {
            return parse<PositionExpr>(begin, end);
        } else if (ident == "vx" || ident == "vy" || ident == "vz") {
            return parse<VelocityExpr>(begin, end);
        } else {
            throw selection_error("unknown operation '{}'", ident);
        }
    } else if (begin->is_ident()) {
        auto ident = begin->ident();
        if (ident == "all") {
            return parse<AllExpr>(begin, end);
        } else if (ident == "none") {
            return parse<NoneExpr>(begin, end);
        } else {
            throw selection_error("unknown operation '{}'", ident);
        }
    } else {
        throw selection_error("could not parse the selection");
    }
}

Ast selections::parse(std::vector<Token> tokens) {
    tokens = add_missing_equals(tokens);
    tokens = shunting_yard(std::move(tokens));

    auto begin = tokens.cbegin();
    const auto end = tokens.cend();
    auto ast = dispatch_parsing(begin, end);

    if (begin != end) {
        throw selection_error("could not parse the end of the selection");
    }

    return ast;
}
