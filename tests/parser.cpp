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
    }

    SECTION("Numbers") {
        auto token = Token::number(3.4);

        REQUIRE(token.type() == Token::NUMBER);
        CHECK(token.is_number());
        CHECK(token.number() == 3.4);

        CHECK_FALSE(token.is_variable());
        CHECK_FALSE(token.is_ident());

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

        CHECK(tokenize("<")[0].type() == Token::LT);
        CHECK(tokenize("<=")[0].type() == Token::LE);
        CHECK(tokenize(">")[0].type() == Token::GT);
        CHECK(tokenize(">=")[0].type() == Token::GE);
        CHECK(tokenize("==")[0].type() == Token::EQ);
        CHECK(tokenize("!=")[0].type() == Token::NEQ);
    }

    SECTION("Functions") {
        CHECK(tokenize("$9")[0].type() == Token::VARIABLE);
        CHECK(tokenize("$ 9")[0].type() == Token::VARIABLE);

        CHECK_THROWS_AS(tokenize("$ gabo"), SelectionError);
        CHECK_THROWS_AS(tokenize("$"), SelectionError);
        CHECK_THROWS_AS(tokenize("78 $"), SelectionError);
        CHECK_THROWS_AS(tokenize("bhics $"), SelectionError);

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
            "3e456782", // Number is too big for the double type
            "§", // Not accepted characters
            "è",
            "à",
            "ü",
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
        auto ast = "and -> index($1) == 1\n    -> index($1) == 1";
        CHECK(parse(tokenize("index == 1 and index == 1"))->print() == ast);

        ast = "or -> index($1) == 1\n   -> index($1) == 1";
        CHECK(parse(tokenize("index == 1 or index == 1"))->print() == ast);

        ast = "not index($1) == 1";
        CHECK(parse(tokenize("not index == 1"))->print() == ast);

        ast = "and -> index($1) == 1\n    -> not index($1) == 1";
        CHECK(parse(tokenize("index == 1 and not index == 1"))->print() == ast);

        ast = "or -> and -> index($1) == 1\n          -> index($1) == 1\n   -> index($1) == 1";
        CHECK(parse(tokenize("index == 1 and index == 1 or index == 1"))->print() == ast);

        ast = "and -> index($1) == 1\n    -> or -> index($1) == 1\n          -> index($1) == 1";
        CHECK(parse(tokenize("index == 1 and (index == 1 or index == 1)"))->print() == ast);

        CHECK_THROWS_AS(parse(tokenize("element H and")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("element <= 4 and")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("and element H")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("and element <= 4")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("element <= 4 or")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("or element <= 4")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("not")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("not element <= 4")), SelectionError);
    }

    SECTION("all & none") {
        CHECK(parse(tokenize("all"))->print() == "all");
        CHECK(parse(tokenize("none"))->print() == "none");

        auto ast = "or -> all\n   -> element($1) == H";
        CHECK(parse(tokenize("all or element H"))->print() == ast);

        ast = "or -> element($1) == H\n   -> none";
        CHECK(parse(tokenize("element H or none"))->print() == ast);

        CHECK(parse(tokenize("not all"))->print() == "not all");
    }

    SECTION("element") {
        CHECK(parse(tokenize("element == goo"))->print() == "element($1) == goo");
        CHECK(parse(tokenize("element($1) == goo"))->print() == "element($1) == goo");
        CHECK(parse(tokenize("element goo"))->print() == "element($1) == goo");
        CHECK(parse(tokenize("element($3) goo"))->print() == "element($3) == goo");
        CHECK(parse(tokenize("element != goo"))->print() == "element($1) != goo");

        CHECK_THROWS_AS(parse(tokenize("element < bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("element >= bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("element == 45")), SelectionError);
    }

    SECTION("name") {
        CHECK(parse(tokenize("name == goo"))->print() == "name($1) == goo");
        CHECK(parse(tokenize("name($1) == goo"))->print() == "name($1) == goo");
        CHECK(parse(tokenize("name goo"))->print() == "name($1) == goo");
        CHECK(parse(tokenize("name($3) goo"))->print() == "name($3) == goo");
        CHECK(parse(tokenize("name != goo"))->print() == "name($1) != goo");

        CHECK_THROWS_AS(parse(tokenize("name < bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("name >= bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("name == 45")), SelectionError);
    }

    SECTION("Index") {
        CHECK(parse(tokenize("index == 4"))->print() == "index($1) == 4");
        CHECK(parse(tokenize("index($1) == 4"))->print() == "index($1) == 4");
        CHECK(parse(tokenize("index 5"))->print() == "index($1) == 5");
        CHECK(parse(tokenize("index($2) 5"))->print() == "index($2) == 5");

        CHECK(parse(tokenize("index <= 42"))->print() == "index($1) <= 42");
        CHECK(parse(tokenize("index != 12"))->print() == "index($1) != 12");

        CHECK_THROWS_AS(parse(tokenize("index == bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("index >= 42.3")), SelectionError);
    }

    SECTION("Mass") {
        CHECK(parse(tokenize("mass == 4"))->print() == "mass($1) == 4.000000");
        CHECK(parse(tokenize("mass($1) == 4"))->print() == "mass($1) == 4.000000");
        CHECK(parse(tokenize("mass 5"))->print() == "mass($1) == 5.000000");
        CHECK(parse(tokenize("mass($2) 5"))->print() == "mass($2) == 5.000000");

        CHECK(parse(tokenize("mass <= 42"))->print() == "mass($1) <= 42.000000");
        CHECK(parse(tokenize("mass != 12"))->print() == "mass($1) != 12.000000");

        CHECK_THROWS_AS(parse(tokenize("mass <= bar")), SelectionError);
    }

    SECTION("Position & velocity") {
        CHECK(parse(tokenize("x == 4"))->print() == "x($1) == 4.000000");
        CHECK(parse(tokenize("x($1) == 4"))->print() == "x($1) == 4.000000");
        CHECK(parse(tokenize("y < 4"))->print() == "y($1) < 4.000000");
        CHECK(parse(tokenize("z >= 4"))->print() == "z($1) >= 4.000000");

        CHECK(parse(tokenize("vx == 4"))->print() == "vx($1) == 4.000000");
        CHECK(parse(tokenize("vy < 4"))->print() == "vy($1) < 4.000000");
        CHECK(parse(tokenize("vz >= 4"))->print() == "vz($1) >= 4.000000");

        CHECK_THROWS_AS(parse(tokenize("x <= bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("vy > bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("z != bar")), SelectionError);
        CHECK_THROWS_AS(parse(tokenize("vx == bar")), SelectionError);
    }

    SECTION("Multiple selections") {
        auto ast = "and -> mass($1) < 4.000000\n    -> element($3) == O";
        CHECK(parse(tokenize("mass($1) < 4 and element($3) O"))->print() == ast);
        ast = "element($4) != Cs";
        CHECK(parse(tokenize("element($4) != Cs"))->print() == ast);
        ast = "or -> index($1) < 4\n   -> element($2) == H";
        CHECK(parse(tokenize("index($1) < 4 or element($2) H"))->print() == ast);
    }

    SECTION("Parsing errors") {
        std::vector<std::string> parse_fail = {
            /* Giberish at the end of the selection */
            "index == 23 6",
            "index == 23 njzk",
            "index == 23 !=",
            "index == 23 element == 1",
            /* Bad usage of the boolean operators */
            "index == 23 and ",
            "and index == 23",
            "not and index == 23",
            "index == 23 or ",
            "or index == 23",
            "not or index == 23",
            "index == 23 not index == 1",
            /* element name expressions */
            "element == <",
            "element == 56",
            "element < foo",
            "element 56",
            "element >=",
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
        };

        for (auto& failure: parse_fail) {
            auto toks = tokenize(failure);
            CHECK_THROWS_AS(parse(toks), SelectionError);
        }
    }
}
