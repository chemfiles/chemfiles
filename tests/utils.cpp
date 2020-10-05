// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include "chemfiles/utils.hpp"
#include "chemfiles/Error.hpp"

TEST_CASE("ASCII utils") {
    SECTION("is_letter") {
        CHECK(chemfiles::is_ascii_letter('a'));
        CHECK(chemfiles::is_ascii_letter('G'));

        CHECK_FALSE(chemfiles::is_ascii_letter('2'));
        CHECK_FALSE(chemfiles::is_ascii_letter('!'));
        CHECK_FALSE(chemfiles::is_ascii_letter(' '));
        CHECK_FALSE(chemfiles::is_ascii_letter('\n'));
        CHECK_FALSE(chemfiles::is_ascii_letter("à"[0]));
        CHECK_FALSE(chemfiles::is_ascii_letter("叁"[0]));
    }

    SECTION("is_lowercase") {
        CHECK(chemfiles::is_ascii_lowercase('a'));
        CHECK(chemfiles::is_ascii_lowercase('f'));
        CHECK(chemfiles::is_ascii_lowercase('b'));

        CHECK_FALSE(chemfiles::is_ascii_lowercase('G'));
        CHECK_FALSE(chemfiles::is_ascii_lowercase('L'));
        CHECK_FALSE(chemfiles::is_ascii_lowercase('2'));
        CHECK_FALSE(chemfiles::is_ascii_lowercase('!'));
        CHECK_FALSE(chemfiles::is_ascii_lowercase(' '));
        CHECK_FALSE(chemfiles::is_ascii_lowercase('\n'));
        CHECK_FALSE(chemfiles::is_ascii_lowercase("à"[0]));
        CHECK_FALSE(chemfiles::is_ascii_lowercase("叁"[0]));
    }

    SECTION("is_uppercase") {
        CHECK(chemfiles::is_ascii_uppercase('A'));
        CHECK(chemfiles::is_ascii_uppercase('F'));
        CHECK(chemfiles::is_ascii_uppercase('B'));

        CHECK_FALSE(chemfiles::is_ascii_uppercase('g'));
        CHECK_FALSE(chemfiles::is_ascii_uppercase('l'));
        CHECK_FALSE(chemfiles::is_ascii_uppercase('2'));
        CHECK_FALSE(chemfiles::is_ascii_uppercase('!'));
        CHECK_FALSE(chemfiles::is_ascii_uppercase(' '));
        CHECK_FALSE(chemfiles::is_ascii_uppercase('\n'));
        CHECK_FALSE(chemfiles::is_ascii_uppercase("à"[0]));
        CHECK_FALSE(chemfiles::is_ascii_uppercase("叁"[0]));
    }

    SECTION("is_digit") {
        CHECK(chemfiles::is_ascii_digit('0'));
        CHECK(chemfiles::is_ascii_digit('1'));
        CHECK(chemfiles::is_ascii_digit('2'));
        CHECK(chemfiles::is_ascii_digit('3'));
        CHECK(chemfiles::is_ascii_digit('4'));
        CHECK(chemfiles::is_ascii_digit('5'));
        CHECK(chemfiles::is_ascii_digit('6'));
        CHECK(chemfiles::is_ascii_digit('7'));
        CHECK(chemfiles::is_ascii_digit('8'));
        CHECK(chemfiles::is_ascii_digit('9'));

        CHECK_FALSE(chemfiles::is_ascii_digit('a'));
        CHECK_FALSE(chemfiles::is_ascii_digit('Z'));
        CHECK_FALSE(chemfiles::is_ascii_digit('!'));
        CHECK_FALSE(chemfiles::is_ascii_digit(' '));
        CHECK_FALSE(chemfiles::is_ascii_digit('\n'));
        CHECK_FALSE(chemfiles::is_ascii_digit("à"[0]));
        CHECK_FALSE(chemfiles::is_ascii_digit("叁"[0]));
    }

    SECTION("is_alphanumeric") {
        CHECK(chemfiles::is_ascii_alphanumeric('2'));
        CHECK(chemfiles::is_ascii_alphanumeric('9'));
        CHECK(chemfiles::is_ascii_alphanumeric('a'));
        CHECK(chemfiles::is_ascii_alphanumeric('Z'));

        CHECK_FALSE(chemfiles::is_ascii_alphanumeric('!'));
        CHECK_FALSE(chemfiles::is_ascii_alphanumeric(' '));
        CHECK_FALSE(chemfiles::is_ascii_alphanumeric('\n'));
        CHECK_FALSE(chemfiles::is_ascii_alphanumeric("à"[0]));
        CHECK_FALSE(chemfiles::is_ascii_alphanumeric("叁"[0]));
    }

    SECTION("whitespace") {
        CHECK(chemfiles::is_ascii_whitespace(' '));
        CHECK(chemfiles::is_ascii_whitespace('\t'));
        CHECK(chemfiles::is_ascii_whitespace('\n'));
        CHECK(chemfiles::is_ascii_whitespace('\x0C'));

        CHECK_FALSE(chemfiles::is_ascii_whitespace('\v'));
        CHECK_FALSE(chemfiles::is_ascii_whitespace('4'));
        CHECK_FALSE(chemfiles::is_ascii_whitespace('a'));
        CHECK_FALSE(chemfiles::is_ascii_whitespace('Z'));
        CHECK_FALSE(chemfiles::is_ascii_whitespace('!'));
        CHECK_FALSE(chemfiles::is_ascii_whitespace("à"[0]));
        CHECK_FALSE(chemfiles::is_ascii_whitespace("叁"[0]));
    }

    SECTION("to_ascii_uppercase") {
        std::string string = "Test à 叁 4535\nBeBa";
        chemfiles::to_ascii_uppercase(string);
        CHECK(string == "TEST à 叁 4535\nBEBA");
    }

    SECTION("to_ascii_lowercase") {
        std::string string = "Test à 叁 4535\nBeBa";
        chemfiles::to_ascii_lowercase(string);
        CHECK(string == "test à 叁 4535\nbeba");
    }
}

TEST_CASE("trim") {
    CHECK(chemfiles::trim("   ") == "");
    CHECK(chemfiles::trim("   left") == "left");
    CHECK(chemfiles::trim("right    ") == "right");
    CHECK(chemfiles::trim("   both   \t") == "both");
    CHECK(chemfiles::trim("tabs\t") == "tabs");
    CHECK(chemfiles::trim("lines\n") == "lines");
    CHECK(chemfiles::trim("lines\r") == "lines");
    CHECK(chemfiles::trim("lines\r\n") == "lines");
    CHECK(chemfiles::trim("\r\nlines\r") == "lines");
    CHECK(chemfiles::trim("   and \t with\rsome\n   inside  \t") == "and \t with\rsome\n   inside");
}

TEST_CASE("split") {
    auto expected = std::vector<chemfiles::string_view>{"bla", "bla  bla, jk", "fiuks"};
    CHECK(chemfiles::split("bla:bla  bla, jk:fiuks", ':') == expected);

    expected = std::vector<chemfiles::string_view>{"bla  bla", " jk:fiuks"};
    CHECK(chemfiles::split(",,bla  bla, jk:fiuks", ',') == expected);
}
