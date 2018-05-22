// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/selections/lexer.hpp"
#include "chemfiles/selections/parser.hpp"
#include "chemfiles/selections/expr.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/ErrorFmt.hpp"

#include <functional>
#include <map>

constexpr double PI = 3.141592653589793238463;

using namespace chemfiles;
using namespace chemfiles::selections;

using string_prop_creator_t = std::function<Ast(std::string, bool, Variable)>;
static std::map<std::string, string_prop_creator_t> STRING_PROPERTIES = {
    {"name", [](std::string value, bool equals, Variable var){ return Ast(new Name(value, equals, var));}},
    {"type", [](std::string value, bool equals, Variable var){ return Ast(new Type(value, equals, var));}},
    {"resname", [](std::string value, bool equals, Variable var){ return Ast(new Resname(value, equals, var));}},
};

using num_prop_creator_t = std::function<MathAst(Variable)>;
static std::map<std::string, num_prop_creator_t> NUMERIC_PROPERTIES = {
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

using num_functions_creator_t = std::function<MathAst(MathAst)>;
static std::map<std::string, num_functions_creator_t> NUMERIC_FUNCTIONS = {
    {"sin", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(sin), "sin", std::move(ast)));}},
    {"cos", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(cos), "cos", std::move(ast)));}},
    {"tan", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(tan), "tan", std::move(ast)));}},
    {"asin", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(asin), "asin", std::move(ast)));}},
    {"acos", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(acos), "acos", std::move(ast)));}},
    {"sqrt", [](MathAst ast){ return MathAst(new Function(static_cast<double (*)(double)>(sqrt), "sqrt", std::move(ast)));}},
    {"rad2deg", [](MathAst ast){ return MathAst(new Function([](double rad){ return rad * 180 / PI; }, "rad2deg", std::move(ast)));}},
    {"deg2rad", [](MathAst ast){ return MathAst(new Function([](double deg){ return deg * PI / 180; }, "deg2rad", std::move(ast)));}},
};

using num_var_functions_creator_t = std::function<MathAst(std::vector<Variable>)>;
struct NumericVarFunction {
    unsigned arity;
    num_var_functions_creator_t creator;
};

static std::map<std::string, NumericVarFunction> NUMERIC_VAR_FUNCTIONS = {
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
    {"all", {0, [](std::vector<SubSelection>){ return Ast(new All()); }}},
    {"none", {0, [](std::vector<SubSelection>){ return Ast(new None()); }}},
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


static bool is_string_property(const std::string& name) {
    return STRING_PROPERTIES.find(name) != STRING_PROPERTIES.end();
}

static bool is_numeric_property(const std::string& name) {
    return NUMERIC_PROPERTIES.find(name) != NUMERIC_PROPERTIES.end();
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
    current_ = 0;
    auto ast = expression();
    if (!finished()) {
        std::string extra;
        while (!finished()) {
            extra += " " + advance().str();
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
        auto ast = expression();
        if (match(Token::RPAREN)) {
            return ast;
        } else {
            throw SelectionError("mismatched parenthesis");
        }
    } else if (match(Token::NOT)) {
        auto ast = expression();
        return Ast(new Not(std::move(ast)));
    } else if (check(Token::IDENT)) {
        auto ident = peek().ident();
        if (is_boolean_selector(ident)) {
            return bool_selector();
        } else if (is_string_property(ident)) {
            return string_selector();
        } else {
            return math_selector();
        }
    } else {
        // If everything else fails, try to parse it as mathematical expression
        return math_selector();
    }
}

Ast Parser::bool_selector() {
    auto token = advance();
    assert(token.type() == Token::IDENT);
    auto name = token.ident();
    assert(is_boolean_selector(name));

    auto selector = BOOLEAN_SELECTORS[name];

    auto arguments = sub_selection();
    if (arguments.size() != selector.arity) {
        throw selection_error("expected {} arguments in call to {}, got {}",
            arguments.size(), name, selector.arity
        );
    }

    // Check that at least one of the argument is a variable. Else, selections
    // like `bonded(name H, name O)` are equivalent to either all or none
    // depending on the system.
    bool at_least_one_variable = false;
    for (auto& arg: arguments) {
        at_least_one_variable = at_least_one_variable || arg.is_variable();
    }
    if (!arguments.empty() && !at_least_one_variable) {
        throw selection_error("expected at least one variable (#1/#2/#3/#4) in call to {}", name);
    }

    return selector.creator(std::move(arguments));
}


Ast Parser::string_selector() {
    auto property = advance();
    assert(property.type() == Token::IDENT);
    auto name = property.ident();
    assert(is_string_property(name));

    auto var = variable();
    if (match(Token::IDENT) || match(Token::RAW_IDENT)) {
        // `name value` shortand, where value is a string (e.g. type H, name "42")
        auto value = previous().ident();
        auto ast = STRING_PROPERTIES[name](std::move(value), true, var);
        while (match(Token::IDENT) || match(Token::RAW_IDENT)) {
            // handle multiple values 'name H N C O'
            value = previous().ident();
            auto rhs = STRING_PROPERTIES[name](std::move(value), true, var);
            ast = Ast(new Or(std::move(ast), std::move(rhs)));
        }
        return ast;
    } else if (match(Token::EQUAL)) {
        if (match(Token::IDENT) || match(Token::RAW_IDENT)) {
            auto value = previous().ident();
            return STRING_PROPERTIES[name](std::move(value), true, var);
        } else {
            throw selection_error("expected a value after '{} ==', found {}", name, peek().str());
        }
    } else if (match(Token::NOT_EQUAL)) {
        if (match(Token::IDENT) || match(Token::RAW_IDENT)) {
            auto value = previous().ident();
            return STRING_PROPERTIES[name](std::move(value), false, var);
        } else {
            throw selection_error("expected a value after '{} !=', found {}", name, peek().str());
        }
    } else {
        throw selection_error("expected one of '!=', '==' or a value after {}, found {}", name, peek().str());
    }
}

Ast Parser::math_selector()  {
    auto index = current_;
    if (match(Token::IDENT)) {
        auto name = previous().ident();
        if (is_numeric_property(name)) {
            auto var = variable();
            if (match(Token::NUMBER)) {
                // `name value` shortand, where value is a number
                auto value = previous().number();
                auto math_lhs = NUMERIC_PROPERTIES[name](var);
                auto math_rhs = MathAst(new Number(value));
                auto ast = Ast(new Math(Math::Operator::EQUAL, std::move(math_lhs), std::move(math_rhs)));
                while (match(Token::NUMBER)) {
                    // handle multiple values 'index 7 8 9 11'
                    value = previous().number();
                    math_lhs = NUMERIC_PROPERTIES[name](var);
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
        throw selection_error("expected a binary operator (==, !=, <=, ...), got {}", peek().str());
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
        } else if (is_numeric_property(name)) {
            return math_property(name);
        } else if (is_numeric_var_function(name)) {
            return math_var_function(name);
        } else {
            throw selection_error("unexpected identifier '{}'", name);
        }
    } else if (match(Token::LPAREN)) {
        auto ast = math_sum();
        if (!match(Token::RPAREN)) {
            throw selection_error("mismatched parenthesis");
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
            throw selection_error("expected content after", previous().str());
        } else {
            throw selection_error("I don't know what to do with {}", peek().str());
        }
    }
}

MathAst Parser::math_function(const std::string& name) {
    assert(is_numeric_function(name));
    if (!match(Token::LPAREN)) {
        throw selection_error("missing parenthesis after {}", name);
    }
    auto ast = math_sum();
    if (!match(Token::RPAREN)) {
        throw selection_error("missing closing parenthesis in {} call", name);
    }
    return NUMERIC_FUNCTIONS[name](std::move(ast));
}

MathAst Parser::math_var_function(const std::string& name) {
    assert(is_numeric_var_function(name));
    auto& function = NUMERIC_VAR_FUNCTIONS[name];

    assert(function.arity >= 1);
    auto arguments = variables();
    if (arguments.size() != function.arity) {
        throw selection_error("expected {} arguments in call to {}, got {}",
            arguments.size(), name, function.arity
        );
    }

    return function.creator(arguments);
}

MathAst Parser::math_property(const std::string& name) {
    assert(is_numeric_property(name));
    auto var = variable();
    return NUMERIC_PROPERTIES[name](var);
}

Variable Parser::variable() {
    Variable var = 0;
    if (match(Token::LPAREN)) {
        if (match(Token::VARIABLE)) {
            var = previous().variable();
        } else {
            throw selection_error("expected variable in parenthesis, got {}", peek().str());
        }

        if (!match(Token::RPAREN)) {
            throw selection_error("expected closing parenthesis after variable, got {}", peek().str());
        }
    }
    return var;
}

std::vector<Variable> Parser::variables() {
    std::vector<Variable> vars;
    if (!match(Token::LPAREN)) {
        throw selection_error("expected opening parenthesis, got {}", peek().str());
    }

    if (match(Token::VARIABLE)) {
        vars.push_back(previous().variable());
    } else {
        throw selection_error("expected variable in parenthesis, got {}", peek().str());
    }

    while (match(Token::COMMA)) {
        if (match(Token::VARIABLE)) {
            vars.push_back(previous().variable());
        } else {
            throw selection_error("expected variable in parenthesis, got {}", peek().str());
        }
    }

    if (!match(Token::RPAREN)) {
        throw selection_error("expected closing parenthesis after variable, got {}", peek().str());
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
        vars.push_back(previous().variable());
    } else {
        // HACK: We can not (yet) build a selection directly from AST, because
        // we need to validate the variables and get the context. So we eat all
        // tokens that would make for a selection, turn them back into a string
        // and create a selection from this.
        auto before = current_;
        auto _ast = expression();
        std::string selection;
        for (size_t i=before; i<current_; i++) {
            selection += " " + tokens_[i].str();
        }
        vars.emplace_back(trim(selection));
    }

    while (match(Token::COMMA)) {
        if (match(Token::VARIABLE)) {
            vars.push_back(previous().variable());
        } else {
            auto before = current_;
            auto _ast = expression();
            std::string selection;
            for (size_t i=before; i<current_; i++) {
                selection += " " + tokens_[i].str();
            }
            vars.emplace_back(trim(selection));
        }
    }

    if (!match(Token::RPAREN)) {
        throw selection_error("expected closing parenthesis after variable, got {}", peek().str());
    }

    return vars;
}
