/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */
#include <sstream>
#include <stack>
#include <algorithm>

#include "chemfiles/selections/parser.hpp"
#include "chemfiles/selections/expr.hpp"
#include "chemfiles/Error.hpp"

using namespace chemfiles;
using namespace selections;

/* Standard shunting-yard algorithm, as described in Wikipedia
 * https://en.wikipedia.org/wiki/Shunting-yard_algorithm
 *
 * This convert infix expressions into an AST-like expression, while checking parentheses.
 * The following input:
 *       name == bar and x <= 56
 * is converted to:
 *       and <= 56 x == bar name
 * which is the AST for
 *             and
 *         /          \
 *        ==          <=
 *       /  \        /  \
 *    name   bar    x    56
 */
static std::vector<Token> shunting_yard(token_iterator_t token, token_iterator_t end) {
    std::stack<Token> operators;
    std::vector<Token> output;
    while (token != end) {
        if (token->is_ident() || token->is_number()) {
            output.push_back(*token);
        } else if (token->is_operator()) {
            while (!operators.empty()) {
                // All the operators are left-associative
                if (token->precedence() <= operators.top().precedence()) {
                     output.push_back(operators.top());
                     operators.pop();
                } else {
                    break;
                }
            }
            operators.push(*token);
        } else if (token->type() == Token::LPAREN) {
            operators.push(*token);
        } else if (token->type() == Token::RPAREN) {
            while (!operators.empty() && operators.top().type() != Token::LPAREN) {
                output.push_back(operators.top());
                operators.pop();
            }
            if (operators.empty() || operators.top().type() != Token::LPAREN) {
                throw ParserError("Parentheses mismatched");
            } else {
                operators.pop();
            }
        }
        token++;
    }
    while (!operators.empty()) {
        if (operators.top().type() == Token::LPAREN || operators.top().type() == Token::RPAREN) {
            throw ParserError("Parentheses mismatched");
        } else {
            output.push_back(operators.top());
            operators.pop();
        }
    }
    // AST come out as reverse polish notation, let's reverse it for easier parsing after
    std::reverse(std::begin(output), std::end(output));
    return output;
}

static bool have_short_form(const std::string& expr) {
    return expr == "name" || expr == "index";
}

/* Rewrite the token stream to convert short form for the expressions to the long one.
 *
 * Short forms are expressions like `name foo` or `index 3`, which are equivalent
 * to `name == foo` and `index == 3`.
 */
static std::vector<Token> clean_token_stream(std::vector<Token> stream) {
    auto out = std::vector<Token>();
    for (auto it=stream.cbegin(); it != stream.cend(); it++) {
        if (it->is_ident() && have_short_form(it->ident())) {
            auto next = it + 1;
            if (next != stream.cend() && !next->is_operator()) {
                out.emplace_back(Token(Token::EQ));
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
            throw std::runtime_error("Hit the default case in dispatch_parsing");
        }
    } else if (begin->is_binary_op()) {
        if ((end - begin) < 3 || begin[2].type() != Token::IDENT) {
            std::stringstream tokens;
            for (auto tok = end - 1; tok != begin - 1; tok--) {
                tokens << tok->str() << " ";
            }
            throw ParserError("Bad binary operator: " + tokens.str());
        }

        auto ident = begin[2].ident();
        if (ident == "name") {
            return parse<NameExpr>(begin, end);
        } else if (ident == "index") {
            return parse<IndexExpr>(begin, end);
        } else if (ident == "x" || ident == "y" || ident == "z") {
            return parse<PositionExpr>(begin, end);
        } else if (ident == "vx" || ident == "vy" || ident == "vz") {
            return parse<VelocityExpr>(begin, end);
        } else {
            throw ParserError("Unknown operation: " + ident);
        }
    } else if (begin->is_ident() && begin->ident() == "all") {
        return parse<AllExpr>(begin, end);
    } else {
        throw ParserError("Could not parse the selection");
    }
}

Ast selections::parse(std::vector<Token> token_stream) {
    token_stream = clean_token_stream(token_stream);
    auto rpn = shunting_yard(std::begin(token_stream), std::end(token_stream));

    auto begin = rpn.cbegin();
    const auto end = rpn.cend();
    auto ast = dispatch_parsing(begin, end);

    if (begin != end) throw ParserError("Could not parse the end of the selection.");
    return ast;
}
