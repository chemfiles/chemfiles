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
        auto OPERATORS = std::vector<Token::Type> {
            Token::END, Token::LPAREN, Token::RPAREN, Token::LBRACKET,
            Token::RBRACKET, Token::COMMA, Token::EQUAL, Token::NOT_EQUAL,
            Token::LESS, Token::LESS_EQUAL, Token::GREATER, Token::GREATER_EQUAL,
            Token::PLUS, Token::MINUS, Token::STAR, Token::SLASH, Token::PERCENT,
            Token::NOT, Token::AND, Token::OR
        };

        for (auto type: OPERATORS) {
            auto token = Token(type);
            CHECK(token.type() == type);

            CHECK_THROWS_AS(token.ident(), Error);
            CHECK_THROWS_AS(token.number(), Error);
            CHECK_THROWS_AS(token.variable(), Error);
            CHECK_THROWS_AS(token.string(), Error);
        }

        CHECK(Token(Token::END).as_str() == "<end of selection>");
        CHECK(Token(Token::LPAREN).as_str() == "(");
        CHECK(Token(Token::RPAREN).as_str() == ")");
        CHECK(Token(Token::LBRACKET).as_str() == "[");
        CHECK(Token(Token::RBRACKET).as_str() == "]");
        CHECK(Token(Token::COMMA).as_str() == ",");
        CHECK(Token(Token::EQUAL).as_str() == "==");
        CHECK(Token(Token::NOT_EQUAL).as_str() == "!=");
        CHECK(Token(Token::LESS).as_str() == "<");
        CHECK(Token(Token::LESS_EQUAL).as_str() == "<=");
        CHECK(Token(Token::GREATER).as_str() == ">");
        CHECK(Token(Token::GREATER_EQUAL).as_str() == ">=");
        CHECK(Token(Token::PLUS).as_str() == "+");
        CHECK(Token(Token::MINUS).as_str() == "-");
        CHECK(Token(Token::STAR).as_str() == "*");
        CHECK(Token(Token::SLASH).as_str() == "/");
        CHECK(Token(Token::HAT).as_str() == "^");
        CHECK(Token(Token::PERCENT).as_str() == "%");
        CHECK(Token(Token::NOT).as_str() == "not");
        CHECK(Token(Token::AND).as_str() == "and");
        CHECK(Token(Token::OR).as_str() == "or");
    }

    SECTION("Identifers") {
        auto token = Token::ident("blabla");
        CHECK(token.type() == Token::IDENT);
        CHECK(token.ident() == "blabla");
        CHECK(token.as_str() == "blabla");

        CHECK_THROWS_AS(token.number(), Error);
        CHECK_THROWS_AS(token.variable(), Error);

        CHECK_FALSE(selections::is_ident(""));
        CHECK_FALSE(selections::is_ident("_not_an_id"));
        CHECK_FALSE(selections::is_ident("3not_an_id"));
        CHECK_FALSE(selections::is_ident("not an id"));
        CHECK_FALSE(selections::is_ident("not-an-id"));
    }

    SECTION("Strings") {
        auto token = Token::string("blabla");
        CHECK(token.type() == Token::STRING);
        CHECK(token.string() == "blabla");
        CHECK(token.as_str() == "\"blabla\"");

        CHECK_THROWS_AS(token.number(), Error);
        CHECK_THROWS_AS(token.variable(), Error);
    }

    SECTION("Numbers") {
        auto token = Token::number(3.4);
        CHECK(token.type() == Token::NUMBER);
        CHECK(token.number() == 3.4);
        CHECK(token.as_str() == "3.400000");

        CHECK_THROWS_AS(token.ident(), Error);
        CHECK_THROWS_AS(token.variable(), Error);
        CHECK_THROWS_AS(token.string(), Error);
    }

    SECTION("Variables") {
        auto token = Token::variable(18);
        CHECK(token.type() == Token::VARIABLE);
        CHECK(token.variable() == 18);
        CHECK(token.as_str() == "#19");

        CHECK_THROWS_AS(token.ident(), Error);
        CHECK_THROWS_AS(token.number(), Error);
        CHECK_THROWS_AS(token.string(), Error);
    }

    SECTION("Constructor errors") {
        // These constructor needs additional data
        CHECK_THROWS_AS(Token(Token::NUMBER), Error);
        CHECK_THROWS_AS(Token(Token::IDENT), Error);
        CHECK_THROWS_AS(Token(Token::STRING), Error);
        CHECK_THROWS_AS(Token(Token::VARIABLE), Error);
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

        CHECK_THROWS_WITH(tokenize("22bar == foo"), "identifiers can not start with a digit: '22bar'");
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

    SECTION("Brackets") {
        CHECK(tokenize("[")[0].type() == Token::LBRACKET);
        CHECK(tokenize("]")[0].type() == Token::RBRACKET);

        auto tokens = tokenize("[bagyu");
        CHECK(tokens.size() == 3);
        CHECK(tokens[0].type() == Token::LBRACKET);

        tokens = tokenize("]qbisbszlh");
        CHECK(tokens.size() == 3);
        CHECK(tokens[0].type() == Token::RBRACKET);

        tokens = tokenize("jsqsb[");
        CHECK(tokens.size() == 3);
        CHECK(tokens[1].type() == Token::LBRACKET);

        tokens = tokenize("kjpqhiufn]");
        CHECK(tokens.size() == 3);
        CHECK(tokens[1].type() == Token::RBRACKET);
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
        CHECK(tokenize("%")[0].type() == Token::PERCENT);
    }

    SECTION("Functions") {
        CHECK(tokenize("#9")[0].type() == Token::VARIABLE);
        CHECK(tokenize("#255")[0].type() == Token::VARIABLE);

        CHECK_THROWS_WITH(tokenize("# gabo"), "expected number after #");
        CHECK_THROWS_WITH(tokenize("#"), "expected number after #");
        CHECK_THROWS_WITH(tokenize("bhics #"), "expected number after #");
        CHECK_THROWS_WITH(tokenize("#1844674407370955161555"), "could not parse variable in '1844674407370955161555'");
        CHECK_THROWS_WITH(tokenize("#256"), "variable index #256 is too big (should be less than 255)");
        CHECK_THROWS_WITH(tokenize("#0"), "invalid variable index #0");

        CHECK(tokenize(",")[0].type() == Token::COMMA);
        auto tokens = tokenize(",bagyu");
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0].type() == Token::COMMA);
        CHECK(tokens[1].type() == Token::IDENT);
        CHECK(tokens[2].type() == Token::END);

        tokens = tokenize("jsqsb,");
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0].type() == Token::IDENT);
        CHECK(tokens[1].type() == Token::COMMA);
        CHECK(tokens[2].type() == Token::END);
    }

    SECTION("String") {
        CHECK(tokenize("\"aa\"")[0].type() == Token::STRING);

        for (std::string id: {"\"\"", "\"id_3nt___\"", "\"and\"", "\"3.2\""}) {
            auto tokens = tokenize(id);
            CHECK(tokens.size() == 2);
            CHECK(tokens[0].type() == Token::STRING);
            CHECK(tokens[0].string() == id.substr(1, id.size() - 2));
            CHECK(tokens[1].type() == Token::END);
        }

        CHECK_THROWS_WITH(tokenize("\"foo"), "closing quote (\") not found in '\"foo'");
        CHECK_THROWS_WITH(tokenize("\"foo and name 4"), "closing quote (\") not found in '\"foo and name 4'");
    }
}

TEST_CASE("Lexing errors") {
    std::vector<std::string> LEX_FAIL = {
        "§", // Not accepted characters
        "`",
        "!",
        "&",
        "|",
        "@",
        "#",
        "~",
        "{",
        "}",
        "_",
        "\\",
        "=",
        "è",
        "à",
        "ü",
        "∀",
        "ζ",
        "Ｒ", // weird full width UTF-8 character
        "形",
        "# 9",
        "9.2.5",
    };

    for (auto& failure: LEX_FAIL) {
        CHECK_THROWS_AS(tokenize(failure), SelectionError);
    }
}
