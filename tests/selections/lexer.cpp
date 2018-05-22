// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles/selections/lexer.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

static std::vector<Token> tokenize(std::string selection) {
    return Tokenizer(selection).tokenize();
}

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
        CHECK(token.str() == "blabla");

        CHECK_THROWS_AS(token.number(), Error);
        CHECK_THROWS_AS(token.variable(), Error);

        token = Token::raw_ident("blabla");
        REQUIRE(token.type() == Token::RAW_IDENT);
        CHECK(token.ident() == "blabla");
        CHECK(token.str() == "\"blabla\"");

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
            CHECK(tokenize(str).size() == 2);
        }
        CHECK(tokenize("\t  bar \t    hkqs     ").size() == 3);

        auto tokens = tokenize("3+#4(foo==not<");
        CHECK(tokens.size() == 9);
        CHECK(tokens[0].type() == Token::NUMBER);
        CHECK(tokens[1].type() == Token::PLUS);
        CHECK(tokens[2].type() == Token::VARIABLE);
        CHECK(tokens[3].type() == Token::LPAREN);
        CHECK(tokens[4].type() == Token::IDENT);
        CHECK(tokens[5].type() == Token::EQUAL);
        CHECK(tokens[6].type() == Token::NOT);
        CHECK(tokens[7].type() == Token::LESS);
        CHECK(tokens[8].type() == Token::END);
    }

    SECTION("variables") {
        auto tokens = tokenize("#2 #78");
        CHECK(tokens.size() == 3);
        CHECK(tokens[0].type() == Token::VARIABLE);
        CHECK(tokens[0].variable() == 1);
        CHECK(tokens[1].type() == Token::VARIABLE);
        CHECK(tokens[1].variable() == 77);
        CHECK(tokens[2].type() == Token::END);

        CHECK_THROWS_AS(tokenize("#0"), SelectionError);
    }

    SECTION("Identifiers") {
        for (auto& id: {"ident", "id_3nt___", "iD_3BFAMC8T3Vt___"}) {
            auto tokens = tokenize(id);
            CHECK(tokens.size() == 2);
            CHECK(tokens[0].type() == Token::IDENT);
            CHECK(tokens[0].ident() == id);
            CHECK(tokens[1].type() == Token::END);
        }

        for (std::string id: {"\"\"", "\"id_3nt___\"", "\"and\"", "\"3.2\""}) {
            auto tokens = tokenize(id);
            CHECK(tokens.size() == 2);
            CHECK(tokens[0].type() == Token::RAW_IDENT);
            CHECK(tokens[0].ident() == id.substr(1, id.size() - 2));
            CHECK(tokens[1].type() == Token::END);
        }
    }

    SECTION("Numbers") {
        for (auto& str: {"4", "567.34", "452.1E4", "4e+5", "4.6784e-56"}) {
            auto tokens = tokenize(str);
            CHECK(tokens.size() == 2);
            CHECK(tokens[0].type() == Token::NUMBER);
            CHECK(tokens[1].type() == Token::END);
        }

        /// A bit of a weird case, but this should be handled too
        auto tokens = tokenize("3e+5+6");
        CHECK(tokens.size() == 4);
        CHECK(tokens[0].type() == Token::NUMBER);
        CHECK(tokens[0].number() == 3e+5);
        CHECK(tokens[1].type() == Token::PLUS);
        CHECK(tokens[2].type() == Token::NUMBER);
        CHECK(tokens[2].number() == 6);
        CHECK(tokens[3].type() == Token::END);
    }

    SECTION("Parentheses") {
        CHECK(tokenize("(")[0].type() == Token::LPAREN);
        CHECK(tokenize(")")[0].type() == Token::RPAREN);

        auto tokens = tokenize("(bagyu");
        CHECK(tokens.size() == 3);
        CHECK(tokens[0].type() == Token::LPAREN);

        tokens = tokenize(")qbisbszlh");
        CHECK(tokens.size() == 3);
        CHECK(tokens[0].type() == Token::RPAREN);

        tokens = tokenize("jsqsb(");
        CHECK(tokens.size() == 3);
        CHECK(tokens[1].type() == Token::LPAREN);

        tokens = tokenize("kjpqhiufn)");
        CHECK(tokens.size() == 3);
        CHECK(tokens[1].type() == Token::RPAREN);
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
        CHECK(tokenize("#255")[0].type() == Token::VARIABLE);

        CHECK_THROWS_AS(tokenize("# gabo"), SelectionError);
        CHECK_THROWS_AS(tokenize("#"), SelectionError);
        CHECK_THROWS_AS(tokenize("78 #"), SelectionError);
        CHECK_THROWS_AS(tokenize("bhics #"), SelectionError);
        CHECK_THROWS_AS(tokenize("#256"), SelectionError);

        CHECK(tokenize(",")[0].type() == Token::COMMA);
        auto tokens = tokenize(",bagyu");
        CHECK(tokens.size() == 3);
        CHECK(tokens[0].type() == Token::COMMA);

        tokens = tokenize("jsqsb,");
        CHECK(tokens.size() == 3);
        CHECK(tokens[1].type() == Token::COMMA);
    }
}

TEST_CASE("Lexing errors") {
    std::vector<std::string> LEX_FAIL = {
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
        "# 9",
        "9.2.5",
    };

    for (auto& failure: LEX_FAIL) {
        CHECK_THROWS_AS(tokenize(failure), SelectionError);
    }
}
