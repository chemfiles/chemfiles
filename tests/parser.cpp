// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles.hpp"
#include "chemfiles/selections/lexer.hpp"
#include "chemfiles/selections/parser.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

TEST_CASE("Tokens", "[selection]") {
    SECTION("Operators") {
        CHECK(Token(Token::AND).is_boolean_op());
        CHECK(Token(Token::OR).is_boolean_op());
        CHECK(Token(Token::NOT).is_boolean_op());

        CHECK(Token(Token::AND).is_operator());
        CHECK(Token(Token::OR).is_operator());
        CHECK(Token(Token::NOT).is_operator());

        CHECK_FALSE(Token(Token::AND).is_binary_op());
        CHECK_FALSE(Token(Token::OR).is_binary_op());
        CHECK_FALSE(Token(Token::NOT).is_binary_op());

        CHECK(Token(Token::EQ).is_operator());
        CHECK(Token(Token::NEQ).is_operator());
        CHECK(Token(Token::LT).is_operator());
        CHECK(Token(Token::LE).is_operator());
        CHECK(Token(Token::GT).is_operator());
        CHECK(Token(Token::GE).is_operator());

        CHECK(Token(Token::EQ).is_binary_op());
        CHECK(Token(Token::NEQ).is_binary_op());
        CHECK(Token(Token::LT).is_binary_op());
        CHECK(Token(Token::LE).is_binary_op());
        CHECK(Token(Token::GT).is_binary_op());
        CHECK(Token(Token::GE).is_binary_op());

        CHECK_FALSE(Token(Token::EQ).is_boolean_op());
        CHECK_FALSE(Token(Token::NEQ).is_boolean_op());
        CHECK_FALSE(Token(Token::LT).is_boolean_op());
        CHECK_FALSE(Token(Token::LE).is_boolean_op());
        CHECK_FALSE(Token(Token::GT).is_boolean_op());
        CHECK_FALSE(Token(Token::GE).is_boolean_op());

        SECTION("Precedence") {
            CHECK(Token(Token::GE).precedence() == Token(Token::LT).precedence());
            CHECK(Token(Token::NEQ).precedence() == Token(Token::LE).precedence());

            CHECK(Token(Token::NEQ).precedence() > Token(Token::AND).precedence());
            CHECK(Token(Token::GT).precedence() > Token(Token::OR).precedence());
            CHECK(Token(Token::EQ).precedence() > Token(Token::NOT).precedence());

            CHECK(Token(Token::AND).precedence() > Token(Token::OR).precedence());
            CHECK(Token(Token::AND).precedence() < Token(Token::NOT).precedence());
        }
    }

    SECTION("Parentheses") {
        CHECK_FALSE(Token(Token::LPAREN).is_boolean_op());
        CHECK_FALSE(Token(Token::LPAREN).is_binary_op());
        CHECK_FALSE(Token(Token::LPAREN).is_operator());

        CHECK_FALSE(Token(Token::RPAREN).is_boolean_op());
        CHECK_FALSE(Token(Token::RPAREN).is_binary_op());
        CHECK_FALSE(Token(Token::RPAREN).is_operator());
    }

    SECTION("Identifers") {
        auto token = Token::ident("blabla");

        REQUIRE(token.type() == Token::IDENT);
        CHECK(token.is_ident());
        CHECK(token.ident() == "blabla");

        CHECK_FALSE(token.is_variable());
        CHECK_FALSE(token.is_number());

        CHECK_FALSE(token.is_binary_op());
        CHECK_FALSE(token.is_boolean_op());
        CHECK_FALSE(token.is_operator());

        // Numbers can be used a idents too
        token = Token::number(3);
        REQUIRE(token.type() == Token::NUMBER);
        CHECK(token.is_ident());
        CHECK(token.ident() == "3");
    }

    SECTION("Numbers") {
        auto token = Token::number(3.4);

        REQUIRE(token.type() == Token::NUMBER);
        CHECK(token.is_number());
        CHECK(token.number() == 3.4);

        CHECK_FALSE(token.is_variable());
        CHECK_FALSE(token.is_binary_op());
        CHECK_FALSE(token.is_boolean_op());
        CHECK_FALSE(token.is_operator());
    }

    SECTION("Commas") {
        auto token = Token(Token::COMMA);

        CHECK_FALSE(token.is_variable());
        CHECK_FALSE(token.is_number());
        CHECK_FALSE(token.is_ident());

        CHECK_FALSE(token.is_binary_op());
        CHECK_FALSE(token.is_boolean_op());
        CHECK_FALSE(token.is_operator());
    }

    SECTION("Dollar") {
        auto token = Token::variable(18);

        REQUIRE(token.type() == Token::VARIABLE);
        CHECK(token.is_variable());
        CHECK(token.variable() == 18);

        CHECK_FALSE(token.is_number());
        CHECK_FALSE(token.is_ident());

        CHECK_FALSE(token.is_binary_op());
        CHECK_FALSE(token.is_boolean_op());
        CHECK_FALSE(token.is_operator());
    }
}

TEST_CASE("Lexing", "[selection]") {
    SECTION("Whitespaces") {
        for (auto& str: {"ident", "ident ", "  ident", " \tident   "}) {
            CHECK(tokenize(str).size() == 1);
        }
        CHECK(tokenize("\t  bar \t    hkqs     ").size() == 2);
    }

    SECTION("Identifiers") {
        for (auto& id: {"ident", "id_3nt___", "iD_3BFAMC8T3Vt___", "67"}) {
            auto toks = tokenize(id);
            CHECK(toks.size() == 1);
            CHECK(toks[0].is_ident());
            CHECK(toks[0].ident() == id);
        }
    }

    SECTION("Numbers") {
        for (auto& str: {"4", "-12748255723", "+3", "567.34", "452.1e4", "4.6784e-56"}) {
            auto toks = tokenize(str);
            CHECK(toks.size() == 1);
            CHECK(toks[0].type() == Token::NUMBER);
        }
    }

    SECTION("Parentheses") {
        CHECK(tokenize("(")[0].type() == Token::LPAREN);
        CHECK(tokenize(")")[0].type() == Token::RPAREN);

        auto toks = tokenize("(bagyu");
        CHECK(toks.size() == 2);
        CHECK(toks[0].type() == Token::LPAREN);

        toks = tokenize(")qbisbszlh");
        CHECK(toks.size() == 2);
        CHECK(toks[0].type() == Token::RPAREN);

        toks = tokenize("jsqsb(");
        CHECK(toks.size() == 2);
        CHECK(toks[1].type() == Token::LPAREN);

        toks = tokenize("kjpqhiufn)");
        CHECK(toks.size() == 2);
        CHECK(toks[1].type() == Token::RPAREN);
    }

    SECTION("Operators") {
        CHECK(tokenize("and")[0].type() == Token::AND);
        CHECK(tokenize("or")[0].type() == Token::OR);
        CHECK(tokenize("not")[0].type() == Token::NOT);

        CHECK(tokenize("<")[0].type() == Token::LT);
        CHECK(tokenize("<=")[0].type() == Token::LE);
        CHECK(tokenize(">")[0].type() == Token::GT);
        CHECK(tokenize(">=")[0].type() == Token::GE);
        CHECK(tokenize("==")[0].type() == Token::EQ);
        CHECK(tokenize("!=")[0].type() == Token::NEQ);
    }

    SECTION("Functions") {
        CHECK(tokenize("#9")[0].type() == Token::VARIABLE);
        CHECK(tokenize("# 9")[0].type() == Token::VARIABLE);
        CHECK(tokenize("#255")[0].type() == Token::VARIABLE);

        CHECK_THROWS_AS(tokenize("# gabo"), SelectionError);
        CHECK_THROWS_AS(tokenize("#"), SelectionError);
        CHECK_THROWS_AS(tokenize("78 #"), SelectionError);
        CHECK_THROWS_AS(tokenize("bhics #"), SelectionError);
        CHECK_THROWS_AS(tokenize("#256"), SelectionError);

        CHECK(tokenize(",")[0].type() == Token::COMMA);
        auto toks = tokenize(",bagyu");
        CHECK(toks.size() == 2);
        CHECK(toks[0].type() == Token::COMMA);

        toks = tokenize("jsqsb,");
        CHECK(toks.size() == 2);
        CHECK(toks[1].type() == Token::COMMA);
    }

    SECTION("Lexing errors") {
        std::vector<std::string> lex_fail = {
            "_not_an_id",
            "3not_an_id",
            "§", // Not accepted characters
            "è",
            "à",
            "ü",
            "∀",
            "ζ",
            "Ｒ", // weird full width UTF-8 character
            "形",
            "/",
            "^",
            "`",
            "!",
            "&",
            "|",
            "#",
            "@",
        };

        for (auto& failure: lex_fail) {
            CHECK_THROWS_AS(tokenize(failure), SelectionError);
        }
    }
}

TEST_CASE("Parsing", "[selection]") {
    // This section uses the pretty-printing of AST to check the parsing
    SECTION("Operators") {
        auto ast = "and -> index(#1) == 1\n    -> index(#1) == 1";
        CHECK(parse(tokenize("index == 1 and index == 1"))->print() == ast);

        ast = "or -> index(#1) == 1\n   -> index(#1) == 1";
        CHECK(parse(tokenize("index == 1 or index == 1"))->print() == ast);

        ast = "not index(#1) == 1";
        CHECK(parse(tokenize("not index == 1"))->print() == ast);

        ast = "and -> index(#1) == 1\n    -> not index(#1) == 1";
        CHECK(parse(tokenize("index == 1 and not index == 1"))->print() == ast);

        ast = "or -> and -> index(#1) == 1\n          -> index(#1) == 1\n   -> index(#1) == 1";
        CHECK(parse(tokenize("index == 1 and index == 1 or index == 1"))->print() == ast);

        ast = "and -> index(#1) == 1\n    -> or -> index(#1) == 1\n          -> index(#1) == 1";
        CHECK(parse(tokenize("index == 1 and (index == 1 or index == 1)"))->print() == ast);

        CHECK_THROWS_AS(parse(tokenize("name H and")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("name H and")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("and name H")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("and name H")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("name H or")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("or name H")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("not")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("name not H")), SelectionError);
    }

    SECTION("all & none") {
        CHECK(parse(tokenize("all"))->print() == "all");
        CHECK(parse(tokenize("none"))->print() == "none");

        auto ast = "or -> all\n   -> name(#1) == H";
        CHECK(parse(tokenize("all or name H"))->print() == ast);

        ast = "or -> name(#1) == H\n   -> none";
        CHECK(parse(tokenize("name H or none"))->print() == ast);

        CHECK(parse(tokenize("not all"))->print() == "not all");
    }

    SECTION("type") {
        CHECK(parse(tokenize("type == goo"))->print() == "type(#1) == goo");
        CHECK(parse(tokenize("type(#1) == goo"))->print() == "type(#1) == goo");
        CHECK(parse(tokenize("type goo"))->print() == "type(#1) == goo");
        CHECK(parse(tokenize("type(#3) goo"))->print() == "type(#3) == goo");
        CHECK(parse(tokenize("type != goo"))->print() == "type(#1) != goo");
        CHECK(parse(tokenize("type == 45"))->print() == "type(#1) == 45");

        CHECK_THROWS_AS(parse(tokenize("type < bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("type >= bar")), SelectionError);
    }

    SECTION("name") {
        CHECK(parse(tokenize("name == goo"))->print() == "name(#1) == goo");
        CHECK(parse(tokenize("name(#1) == goo"))->print() == "name(#1) == goo");
        CHECK(parse(tokenize("name goo"))->print() == "name(#1) == goo");
        CHECK(parse(tokenize("name(#3) goo"))->print() == "name(#3) == goo");
        CHECK(parse(tokenize("name != goo"))->print() == "name(#1) != goo");
        CHECK(parse(tokenize("name 45"))->print() == "name(#1) == 45");

        CHECK_THROWS_AS(parse(tokenize("name < bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("name >= bar")), SelectionError);
    }

    SECTION("index") {
        CHECK(parse(tokenize("index == 4"))->print() == "index(#1) == 4");
        CHECK(parse(tokenize("index(#1) == 4"))->print() == "index(#1) == 4");
        CHECK(parse(tokenize("index 5"))->print() == "index(#1) == 5");
        CHECK(parse(tokenize("index(#2) 5"))->print() == "index(#2) == 5");

        CHECK(parse(tokenize("index <= 42"))->print() == "index(#1) <= 42");
        CHECK(parse(tokenize("index != 12"))->print() == "index(#1) != 12");

        CHECK_THROWS_AS(parse(tokenize("index == bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("index >= 42.3")), SelectionError);
    }

    SECTION("resname") {
        CHECK(parse(tokenize("resname == goo"))->print() == "resname(#1) == goo");
        CHECK(parse(tokenize("resname(#1) == goo"))->print() == "resname(#1) == goo");
        CHECK(parse(tokenize("resname goo"))->print() == "resname(#1) == goo");
        CHECK(parse(tokenize("resname(#3) goo"))->print() == "resname(#3) == goo");
        CHECK(parse(tokenize("resname != goo"))->print() == "resname(#1) != goo");
        CHECK(parse(tokenize("resname 45"))->print() == "resname(#1) == 45");

        CHECK_THROWS_AS(parse(tokenize("resname < bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("resname >= bar")), SelectionError);
    }

    SECTION("resid") {
        CHECK(parse(tokenize("resid == 4"))->print() == "resid(#1) == 4");
        CHECK(parse(tokenize("resid(#1) == 4"))->print() == "resid(#1) == 4");
        CHECK(parse(tokenize("resid 5"))->print() == "resid(#1) == 5");
        CHECK(parse(tokenize("resid(#2) 5"))->print() == "resid(#2) == 5");

        CHECK(parse(tokenize("resid <= 42"))->print() == "resid(#1) <= 42");
        CHECK(parse(tokenize("resid != 12"))->print() == "resid(#1) != 12");

        CHECK_THROWS_AS(parse(tokenize("resid == bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("resid >= 42.3")), SelectionError);
    }

    SECTION("mass") {
        CHECK(parse(tokenize("mass == 4"))->print() == "mass(#1) == 4.000000");
        CHECK(parse(tokenize("mass(#1) == 4"))->print() == "mass(#1) == 4.000000");
        CHECK(parse(tokenize("mass 5"))->print() == "mass(#1) == 5.000000");
        CHECK(parse(tokenize("mass(#2) 5"))->print() == "mass(#2) == 5.000000");

        CHECK(parse(tokenize("mass <= 42"))->print() == "mass(#1) <= 42.000000");
        CHECK(parse(tokenize("mass != 12"))->print() == "mass(#1) != 12.000000");

        CHECK_THROWS_AS(parse(tokenize("mass <= bar")), SelectionError);
    }

    SECTION("Position & velocity") {
        CHECK(parse(tokenize("x == 4"))->print() == "x(#1) == 4.000000");
        CHECK(parse(tokenize("x(#1) == 4"))->print() == "x(#1) == 4.000000");
        CHECK(parse(tokenize("y < 4"))->print() == "y(#1) < 4.000000");
        CHECK(parse(tokenize("z >= 4"))->print() == "z(#1) >= 4.000000");

        CHECK(parse(tokenize("vx == 4"))->print() == "vx(#1) == 4.000000");
        CHECK(parse(tokenize("vy < 4"))->print() == "vy(#1) < 4.000000");
        CHECK(parse(tokenize("vz >= 4"))->print() == "vz(#1) >= 4.000000");

        CHECK_THROWS_AS(parse(tokenize("x <= bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("vy > bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("z != bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("vx == bar")), SelectionError);
    }

    SECTION("Multiple selections") {
        auto ast = "and -> mass(#1) < 4.000000\n    -> name(#3) == O";
        CHECK(parse(tokenize("mass(#1) < 4 and name(#3) O"))->print() == ast);
        ast = "name(#4) != Cs";
        CHECK(parse(tokenize("name(#4) != Cs"))->print() == ast);
        ast = "or -> index(#1) < 4\n   -> name(#2) == H";
        CHECK(parse(tokenize("index(#1) < 4 or name(#2) H"))->print() == ast);
    }

    SECTION("Parsing errors") {
        std::vector<std::string> parse_fail = {
            /* Giberish at the end of the selection */
            "index == 23 6",
            "index == 23 njzk",
            "index == 23 !=",
            "index == 23 name == 1",
            /* Bad usage of the boolean operators */
            "index == 23 and ",
            "and index == 23",
            "not and index == 23",
            "index == 23 or ",
            "or index == 23",
            "not or index == 23",
            "index == 23 not index == 1",
            /* name expressions */
            "name == <",
            "name < foo",
            "name >=",
            /* type expressions */
            "type == <",
            "type < foo",
            "type >=",
            /* index expressions */
            "index == <",
            "index == bar",
            "index <=",
            "index bar",
            /* x|y|z expressions */
            "z == <",
            "y == bar",
            "x <=",
            "z bar",
            /* vx|vy|vz expressions */
            "vz == <",
            "vy == bar",
            "vx <=",
            "vz bar",
            // https://github.com/chemfiles/chemfiles/issues/79
            "type(#1) Al and type(#2) O and type(#3) H )",
        };

        for (auto& failure: parse_fail) {
            auto toks = tokenize(failure);
            CHECK_THROWS_AS(parse(toks), SelectionError);
        }
    }
}
