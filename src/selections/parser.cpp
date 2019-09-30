// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "chemfiles/selections/expr.hpp"
#include "chemfiles/selections/lexer.hpp"
#include "chemfiles/selections/parser.hpp"

#include "chemfiles/utils.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/string_view.hpp"

constexpr double PI = 3.141592653589793238463;

using namespace chemfiles::selections;

using string_selector_creator_t = std::function<Ast(std::string, bool, Variable)>;
static std::map<std::string, string_selector_creator_t> STRING_SELECTORS = {
    {"name", [](std::string value, bool equals, Variable var) {
        return Ast(new Name(std::move(value), equals, var));
    }},
    {"type", [](std::string value, bool equals, Variable var) {
        return Ast(new Type(std::move(value), equals, var));
    }},
    {"resname", [](std::string value, bool equals, Variable var) {
        return Ast(new Resname(std::move(value), equals, var));
    }},
};

using numeric_selector_creator_t = std::function<MathAst(Variable)>;
static std::map<std::string, numeric_selector_creator_t> NUMERIC_SELECTORS = {
    {"index", [](Variable var){ return MathAst(new Index(var));}},
    {"mass", [](Variable var){ return MathAst(new Mass(var));}},
    {"resid", [](Variable var){ return MathAst(new Resid(var));}},
    {"x", [](Variable var){ return MathAst(new Position(var, Coordinate::X));}},
    {"y", [](Variable var){ return MathAst(new Position(var, Coordinate::Y));}},
    {"z", [](Variable var){ return MathAst(new Position(var, Coordinate::Z));}},
    {"vx", [](Variable var){ return MathAst(new Velocity(var, Coordinate::X));}},
    {"vy", [](Variable var){ return MathAst(new Velocity(var, Coordinate::Y));}},
    {"vz", [](Variable var){ return MathAst(new Velocity(var, Coordinate::Z));}},
};

using numeric_functions_creator_t = std::function<MathAst(MathAst)>;
static std::map<std::string, numeric_functions_creator_t> NUMERIC_FUNCTIONS = {
    {"sin", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(sin), "sin", std::move(ast)));}},
    {"cos", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(cos), "cos", std::move(ast)));}},
    {"tan", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(tan), "tan", std::move(ast)));}},
    {"asin", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(asin), "asin", std::move(ast)));}},
    {"acos", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(acos), "acos", std::move(ast)));}},
    {"sqrt", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(sqrt), "sqrt", std::move(ast)));}},
    {"rad2deg", [](MathAst ast){ return MathAst(new Function([](double rad){ return rad * 180 / PI; }, "rad2deg", std::move(ast)));}},
    {"deg2rad", [](MathAst ast){ return MathAst(new Function([](double deg){ return deg * PI / 180; }, "deg2rad", std::move(ast)));}},
};

using numeric_variable_functions_creator_t = std::function<MathAst(std::vector<Variable>)>;
struct NumericVariableFunction {
    unsigned arity;
    numeric_variable_functions_creator_t creator;
};

static std::map<std::string, NumericVariableFunction> NUMERIC_VAR_FUNCTIONS = {
    {"distance", {2, [](std::vector<Variable> args){ return MathAst(new Distance(args[0], args[1])); }}},
    {"angle", {3, [](std::vector<Variable> args){ return MathAst(new Angle(args[0], args[1], args[2])); }}},
    {"dihedral", {4, [](std::vector<Variable> args){ return MathAst(new Dihedral(args[0], args[1], args[2], args[3])); }}},
    {"out_of_plane", {4, [](std::vector<Variable> args){ return MathAst(new OutOfPlane(args[0], args[1], args[2], args[3])); }}},
};


using bool_selector_creator_t = std::function<Ast(std::vector<SubSelection>)>;
struct BooleanFunction {
    unsigned arity;
    bool_selector_creator_t creator;
};

static std::map<std::string, BooleanFunction> BOOLEAN_SELECTORS = {
    {"all", {0, [](std::vector<SubSelection> /*unused*/) {
        return Ast(new All());
    }}},
    {"none", {0, [](std::vector<SubSelection> /*unused*/) {
        return Ast(new None());
    }}},
    {"is_bonded", {2, [](std::vector<SubSelection> args){
        return Ast(new IsBonded(std::move(args[0]), std::move(args[1])));
    }}},
    {"is_angle", {3, [](std::vector<SubSelection> args) {
        assert(args.size() == 3);
        return Ast(new IsAngle(std::move(args[0]), std::move(args[1]), std::move(args[2])));
    }}},
    {"is_dihedral", {4, [](std::vector<SubSelection> args) {
        assert(args.size() == 4);
        return Ast(new IsDihedral(std::move(args[0]), std::move(args[1]), std::move(args[2]), std::move(args[3])));
    }}},
    {"is_improper", {4, [](std::vector<SubSelection> args) {
        assert(args.size() == 4);
        return Ast(new IsImproper(std::move(args[0]), std::move(args[1]), std::move(args[2]), std::move(args[3])));
    }}},
};


static bool is_string_selector(const std::string& name) {
    return STRING_SELECTORS.find(name) != STRING_SELECTORS.end();
}

static bool is_numeric_selector(const std::string& name) {
    return NUMERIC_SELECTORS.find(name) != NUMERIC_SELECTORS.end();
}

static bool is_numeric_function(const std::string& name) {
    return NUMERIC_FUNCTIONS.find(name) != NUMERIC_FUNCTIONS.end();
}

static bool is_numeric_var_function(const std::string& name) {
    return NUMERIC_VAR_FUNCTIONS.find(name) != NUMERIC_VAR_FUNCTIONS.end();
}

static bool is_boolean_selector(const std::string& name) {
    return BOOLEAN_SELECTORS.find(name) != BOOLEAN_SELECTORS.end();
}

Ast Parser::parse() {
    // there should always be at least one token (Token::END)
    if (tokens_.size() <= 1) {
        throw selection_error("empty selection");
    }

    current_ = 0;
    auto ast = expression();
    if (!finished()) {
        std::string extra;
        while (!finished()) {
            extra += " " + advance().as_str();
        }
        throw selection_error("additional data after the end of the selection:{}", extra);
    }
    return ast;
}

Ast Parser::expression() {
    auto ast = selector();
    while (true) {
        if (match(Token::AND)) {
            auto rhs = selector();
            ast = Ast(new And(std::move(ast), std::move(rhs)));
        } else if (match(Token::OR)) {
            auto rhs = selector();
            ast = Ast(new Or(std::move(ast), std::move(rhs)));
        } else {
            break;
        }
    }
    return ast;
}

Ast Parser::selector() {
    if (match(Token::LPAREN)) {
        // There is an ambiguity in the grammar: an opening parenthesis can
        // either delimit a logical block "(foo or bar) and baz"; or a
        // mathematical expression "(3 + 5) * 2 < 3". So here we first try to
        // parse a logical expression, and if it fails we backtrack (in the
        // catch block) and try to parse a mathematical expression instead.
        auto index = current_ - 1;
        Ast ast = nullptr;
        try {
            ast = expression();
        } catch (const SelectionError&) {
            current_ = index;
            return math_selector();
        }

        if (match(Token::RPAREN)) {
            return ast;
        } else {
            throw selection_error("expected closing parenthesis after '{}'", previous().as_str());
        }

    } else if (match(Token::NOT)) {
        auto ast = expression();
        return Ast(new Not(std::move(ast)));
    } else if (match(Token::LBRACKET)) {
        auto index = current_ - 1;
        auto ast = bool_or_string_property();
        if (ast != nullptr) {
            return ast;
        }
        // Reset state and try to parse it as a numeric property
        current_ = index;
    } else if (check(Token::IDENT)) {
        auto ident = peek().ident();
        if (is_boolean_selector(ident)) {
            return bool_selector();
        } else if (is_string_selector(ident)) {
            return string_selector();
        } else {
            return math_selector();
        }
    }
    // If everything else fails, try to parse it as mathematical expression
    return math_selector();
}

Ast Parser::bool_or_string_property() {
    assert(previous().type() == Token::LBRACKET);

    std::string property;
    if (check(Token::IDENT) || check(Token::STRING)) {
        property = advance().string();
        if (!match(Token::RBRACKET)) {
            throw selection_error(
                "expected ] after [{}, got {}",
                previous().as_str(), peek().as_str()
            );
        }
    }

    auto var = variable();
    if (match(Token::IDENT) || match(Token::STRING)) {
        // `[name] value` shortand, where value is a string
        auto value = previous().string();
        auto ast = Ast(new StringProperty(property, std::move(value), true, var));
        while (match(Token::IDENT) || match(Token::STRING)) {
            // handle multiple values '[name] H N C O'
            value = previous().string();
            auto rhs = Ast(new StringProperty(property, std::move(value), true, var));
            ast = Ast(new Or(std::move(ast), std::move(rhs)));
        }
        return ast;
    } else if (match(Token::EQUAL)) {
        if (match(Token::IDENT) || match(Token::STRING)) {
            auto value = previous().string();
            return Ast(new StringProperty(property, std::move(value), true, var));
        } else {
            return nullptr;
        }
    } else if (match(Token::NOT_EQUAL)) {
        if (match(Token::IDENT) || match(Token::STRING)) {
            auto value = previous().string();
            return Ast(new StringProperty(property, std::move(value), false, var));
        } else {
            return nullptr;
        }
    } else if (finished() || check(Token::AND) || check(Token::OR)) {
        // Use it as a bool property
        return Ast(new BoolProperty(property, var));
    } else {
        return nullptr;
    }
}

Ast Parser::bool_selector() {
    auto token = advance();
    assert(token.type() == Token::IDENT);
    const auto& name = token.ident();
    assert(is_boolean_selector(name));

    auto selector = BOOLEAN_SELECTORS[name];

    auto arguments = sub_selection();
    if (arguments.size() != selector.arity) {
        throw selection_error("expected {} arguments in '{}', got {}",
            selector.arity, name, arguments.size()
        );
    }

    // Check that at least one of the argument is a variable. Else, selections
    // like `is_bonded(name H, name O)` are equivalent to either all or none
    // depending on the system.
    bool at_least_one_variable = false;
    for (auto& arg: arguments) {
        at_least_one_variable = at_least_one_variable || arg.is_variable();
    }
    if (!arguments.empty() && !at_least_one_variable) {
        throw selection_error("expected at least one variable (#1/#2/#3/#4) in '{}'", name);
    }

    return selector.creator(std::move(arguments));
}


Ast Parser::string_selector() {
    auto property = advance();
    assert(property.type() == Token::IDENT);
    const auto& name = property.ident();
    assert(is_string_selector(name));

    auto var = variable();
    if (match(Token::IDENT) || match(Token::STRING)) {
        // `name value` shortand, where value is a string (e.g. type H, name "42")
        auto value = previous().string();
        auto ast = STRING_SELECTORS[name](std::move(value), true, var);
        while (match(Token::IDENT) || match(Token::STRING)) {
            // handle multiple values 'name H N C O'
            value = previous().string();
            auto rhs = STRING_SELECTORS[name](std::move(value), true, var);
            ast = Ast(new Or(std::move(ast), std::move(rhs)));
        }
        return ast;
    } else if (match(Token::EQUAL)) {
        if (match(Token::IDENT) || match(Token::STRING)) {
            auto value = previous().string();
            return STRING_SELECTORS[name](std::move(value), true, var);
        } else {
            throw selection_error("expected a string value after '{} ==', found {}", name, peek().as_str());
        }
    } else if (match(Token::NOT_EQUAL)) {
        if (match(Token::IDENT) || match(Token::STRING)) {
            auto value = previous().string();
            return STRING_SELECTORS[name](std::move(value), false, var);
        } else {
            throw selection_error("expected a string value after '{} !=', found {}", name, peek().as_str());
        }
    } else {
        throw selection_error(
            "expected one of '!=', '==' or a string value after '{}', found '{}'",
            name, peek().as_str()
        );
    }
}

Ast Parser::math_selector()  {
    auto index = current_;
    if (match(Token::IDENT)) {
        auto name = previous().ident();
        if (is_numeric_selector(name)) {
            auto var = variable();
            if (match(Token::NUMBER)) {
                // `name value` shortand, where value is a number
                auto value = previous().number();
                auto math_lhs = NUMERIC_SELECTORS[name](var);
                auto math_rhs = MathAst(new Number(value));
                auto ast = Ast(new Math(Math::Operator::EQUAL, std::move(math_lhs), std::move(math_rhs)));
                while (match(Token::NUMBER)) {
                    // handle multiple values 'index 7 8 9 11'
                    value = previous().number();
                    math_lhs = NUMERIC_SELECTORS[name](var);
                    math_rhs = MathAst(new Number(value));
                    auto rhs = Ast(new Math(Math::Operator::EQUAL, std::move(math_lhs), std::move(math_rhs)));
                    ast = Ast(new Or(std::move(ast), std::move(rhs)));
                }
                return ast;
            } else {
                // Reset state and continue
                current_ = index;
            }
        } else {
            // Reset state and continue
            current_ = index;
        }
    }

    auto lhs = math_sum();

    Math::Operator op;
    if (match(Token::EQUAL)) {
        op = Math::Operator::EQUAL;
    } else if (match(Token::NOT_EQUAL)) {
        op = Math::Operator::NOT_EQUAL;
    } else if (match(Token::LESS)) {
        op = Math::Operator::LESS;
    } else if (match(Token::LESS_EQUAL)) {
        op = Math::Operator::LESS_EQUAL;
    } else if (match(Token::GREATER)) {
        op = Math::Operator::GREATER;
    } else if (match(Token::GREATER_EQUAL)) {
        op = Math::Operator::GREATER_EQUAL;
    } else {
        throw selection_error("expected a binary operator (==, !=, <=, ...), got {}", peek().as_str());
    }

    auto rhs = math_sum();
    return Ast(new Math(op, std::move(lhs), std::move(rhs)));
}

MathAst Parser::math_sum() {
    auto ast = math_product();
    while (true) {
        if (match(Token::PLUS)) {
            auto rhs = math_product();
            ast = MathAst(new Add(std::move(ast), std::move(rhs)));
        } else if (match(Token::MINUS)) {
            auto rhs = math_product();
            ast = MathAst(new Sub(std::move(ast), std::move(rhs)));
        } else {
            break;
        }
    }
    return ast;
}

MathAst Parser::math_product() {
    auto ast = math_power();
    while (true) {
        if (match(Token::STAR)) {
            auto rhs = math_power();
            ast = MathAst(new Mul(std::move(ast), std::move(rhs)));
        } else if (match(Token::SLASH)) {
            auto rhs = math_power();
            ast = MathAst(new Div(std::move(ast), std::move(rhs)));
        } else if (match(Token::PERCENT)) {
            auto rhs = math_power();
            ast = MathAst(new Mod(std::move(ast), std::move(rhs)));
        } else {
            break;
        }
    }
    return ast;
}

MathAst Parser::math_power() {
    auto lhs = math_value();
    if (match(Token::HAT)) {
        auto rhs = math_power();
        return MathAst(new Pow(std::move(lhs), std::move(rhs)));
    } else {
        return lhs;
    }
}

MathAst Parser::math_value() {
    if (match(Token::IDENT)) {
        auto name = previous().ident();
        if (is_numeric_function(name)) {
            return math_function(name);
        } else if (is_numeric_selector(name)) {
            auto var = variable();
            return NUMERIC_SELECTORS[name](var);
        } else if (is_numeric_var_function(name)) {
            return math_var_function(name);
        } else {
            throw selection_error("unexpected identifier '{}' in mathematical expression", name);
        }
    } else if (match(Token::LBRACKET)) {
        if (check(Token::IDENT) || check(Token::STRING)) {
            auto property = advance().string();
            if (!match(Token::RBRACKET)) {
                throw selection_error(
                    "expected ] after [{}, got {}",
                    previous().as_str(), peek().as_str()
                );
            }
            auto var = variable();
            return MathAst(new NumericProperty(std::move(property), var));
        } else {
            throw selection_error("expected property name after [, got {}", peek().as_str());
        }
    } else if (match(Token::LPAREN)) {
        auto ast = math_sum();
        if (!match(Token::RPAREN)) {
            throw selection_error("expected closing parenthesis after '{}'", previous().as_str());
        }
        return ast;
    } else if (match(Token::NUMBER)) {
        return MathAst(new Number(previous().number()));
    } else if (match(Token::PLUS)) {
        // Unary plus, nothing to do
        return math_value();
    } else if (match(Token::MINUS)) {
        // Unary minus
        auto ast = math_value();
        return MathAst(new Neg(std::move(ast)));
    } else {
        if (finished()) {
            throw selection_error("expected content after '{}'", previous().as_str());
        } else {
            throw selection_error("unexpected content: '{}'", peek().as_str());
        }
    }
}

MathAst Parser::math_function(const std::string& name) {
    assert(is_numeric_function(name));
    if (!match(Token::LPAREN)) {
        throw selection_error("missing parenthesis after '{}' function", name);
    }
    auto ast = math_sum();
    if (!match(Token::RPAREN)) {
        throw selection_error("missing closing parenthesis after '{}' function call", name);
    }
    return NUMERIC_FUNCTIONS[name](std::move(ast));
}

MathAst Parser::math_var_function(const std::string& name) {
    assert(is_numeric_var_function(name));
    auto& function = NUMERIC_VAR_FUNCTIONS[name];

    assert(function.arity >= 1);
    auto arguments = variables();
    if (arguments.size() != function.arity) {
        throw selection_error("expected {} arguments in '{}', got {}",
            function.arity, name, arguments.size()
        );
    }

    return function.creator(arguments);
}

Variable Parser::variable() {
    Variable var = 0;
    if (match(Token::LPAREN)) {
        if (match(Token::VARIABLE)) {
            var = previous().variable();
        } else {
            throw selection_error("expected variable in parenthesis, got '{}'", peek().as_str());
        }

        if (!match(Token::RPAREN)) {
            throw selection_error("expected closing parenthesis after variable, got '{}'", peek().as_str());
        }
    }
    return var;
}

std::vector<Variable> Parser::variables() {
    std::vector<Variable> vars;
    if (!match(Token::LPAREN)) {
        throw selection_error("expected opening parenthesis, got '{}'", peek().as_str());
    }

    if (match(Token::VARIABLE)) {
        vars.push_back(previous().variable());
    } else {
        throw selection_error("expected variable in parenthesis, got '{}'", peek().as_str());
    }

    while (match(Token::COMMA)) {
        if (match(Token::VARIABLE)) {
            vars.push_back(previous().variable());
        } else {
            throw selection_error("expected variable in parenthesis, got '{}'", peek().as_str());
        }
    }

    if (!match(Token::RPAREN)) {
        throw selection_error("expected closing parenthesis after variable, got '{}'", peek().as_str());
    }

    return vars;
}

std::vector<SubSelection> Parser::sub_selection() {
    std::vector<SubSelection> vars;
    if (!match(Token::LPAREN)) {
        // no arguments
        return vars;
    }

    if (match(Token::VARIABLE)) {
        vars.emplace_back(previous().variable());
    } else {
        // HACK: We can not (yet) build a selection directly from AST, because
        // we need to validate the variables and get the context. So we eat all
        // tokens that would make for a selection, turn them back into a string
        // and create a selection from this.
        auto before = current_;
        auto _ast = expression();
        std::string selection;
        for (size_t i=before; i<current_; i++) {
            selection += " " + tokens_[i].as_str();
        }
        vars.emplace_back(trim(selection).to_string());
    }

    while (match(Token::COMMA)) {
        if (match(Token::VARIABLE)) {
            vars.emplace_back(previous().variable());
        } else {
            auto before = current_;
            auto _ast = expression();
            std::string selection;
            for (size_t i=before; i<current_; i++) {
                selection += " " + tokens_[i].as_str();
            }
            vars.emplace_back(trim(selection).to_string());
        }
    }

    if (!match(Token::RPAREN)) {
        throw selection_error("expected closing parenthesis after variable, got '{}'", peek().as_str());
    }

    return vars;
}
