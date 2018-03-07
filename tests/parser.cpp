// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles.hpp"
#include "chemfiles/selections/lexer.hpp"
#include "chemfiles/selections/parser.hpp"
#include "chemfiles/selections/expr.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

TEST_CASE("Tokens") {
    SECTION("Operators") {
        auto token = Token(Token::LESS_EQUAL);
        REQUIRE(token.type() == Token::LESS_EQUAL);

        CHECK_THROWS_AS(token.ident(), Error);
        CHECK_THROWS_AS(token.number(), Error);
        CHECK_THROWS_AS(token.variable(), Error);
    }

    SECTION("Identifers") {
        auto token = Token::ident("blabla");
        REQUIRE(token.type() == Token::IDENT);
        CHECK(token.ident() == "blabla");

        CHECK_THROWS_AS(token.number(), Error);
        CHECK_THROWS_AS(token.variable(), Error);
    }

    SECTION("Numbers") {
        auto token = Token::number(3.4);
        REQUIRE(token.type() == Token::NUMBER);
        CHECK(token.number() == 3.4);

        CHECK_THROWS_AS(token.ident(), Error);
        CHECK_THROWS_AS(token.variable(), Error);
    }

    SECTION("Variables") {
        auto token = Token::variable(18);
        REQUIRE(token.type() == Token::VARIABLE);
        CHECK(token.variable() == 18);

        CHECK_THROWS_AS(token.ident(), Error);
        CHECK_THROWS_AS(token.number(), Error);
    }
}

TEST_CASE("Lexing") {
    SECTION("Whitespaces") {
        for (auto& str: {"ident", "ident ", "  ident", " \tident   "}) {
            CHECK(tokenize(str).size() == 1);
        }
        CHECK(tokenize("\t  bar \t    hkqs     ").size() == 2);
    }

    SECTION("Identifiers") {
        for (auto& id: {"ident", "id_3nt___", "iD_3BFAMC8T3Vt___"}) {
            auto toks = tokenize(id);
            CHECK(toks.size() == 1);
            CHECK(toks[0].type() == Token::IDENT);
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

        CHECK(tokenize("<")[0].type() == Token::LESS);
        CHECK(tokenize("<=")[0].type() == Token::LESS_EQUAL);
        CHECK(tokenize(">")[0].type() == Token::GREATER);
        CHECK(tokenize(">=")[0].type() == Token::GREATER_EQUAL);
        CHECK(tokenize("==")[0].type() == Token::EQUAL);
        CHECK(tokenize("!=")[0].type() == Token::NOT_EQUAL);

        CHECK(tokenize("+")[0].type() == Token::PLUS);
        CHECK(tokenize("-")[0].type() == Token::MINUS);
        CHECK(tokenize("*")[0].type() == Token::STAR);
        CHECK(tokenize("/")[0].type() == Token::SLASH);
        CHECK(tokenize("^")[0].type() == Token::HAT);
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
            "`",
            "!",
            "&",
            "|",
            "#",
            "@",
            // These are invalid for now, due to the requirement of spaces
            // around operators. This should be lifted soon.
            "42+53",
            "4.2/name",
            "+4.2-7",
        };

        for (auto& failure: lex_fail) {
            CHECK_THROWS_AS(tokenize(failure), SelectionError);
        }
    }
}

#if false

TEST_CASE("Parsing") {
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

#endif
