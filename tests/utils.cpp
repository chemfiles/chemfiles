// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;
#include <boost/asio.hpp>
namespace ip = boost::asio::ip;

#include "chemfiles/utils.hpp"
#include "chemfiles/Error.hpp"

TEST_CASE("hostname") {
    auto hostname = ip::host_name();
    CHECK(chemfiles::hostname() == hostname);
}

TEST_CASE("Current directory") {
    auto cwd = fs::current_path();
    CHECK(chemfiles::current_directory() == cwd.string());
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

TEST_CASE("String parsing") {
    SECTION("Double") {
        CHECK(chemfiles::parse<double>("12.5") == 12.5);
        CHECK(chemfiles::parse<double>("125") == 125);
        CHECK(chemfiles::parse<double>("-32") == -32);

        CHECK_THROWS_AS(chemfiles::parse<double>(""), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<double>("foo"), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<double>("1,2"), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<double>("3e456782"), chemfiles::Error);
    }

    SECTION("long long") {
        CHECK(chemfiles::parse<long long>("125") == 125);
        CHECK(chemfiles::parse<long long>("-32") == -32);

        CHECK_THROWS_AS(chemfiles::parse<long long>(""), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<long long>("foo"), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<long long>("2.5"), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<long long>("9223372036854775808"), chemfiles::Error);
    }

    SECTION("size_t") {
        CHECK(chemfiles::parse<size_t>("125") == 125);

        CHECK_THROWS_AS(chemfiles::parse<size_t>(""), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<size_t>("-32"), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<size_t>("foo"), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<size_t>("2.5"), chemfiles::Error);
        CHECK_THROWS_AS(chemfiles::parse<size_t>("9223372036854775808"), chemfiles::Error);
    }
}

TEST_CASE("scan") {
    int i = 0;
    double d = 0;
    char s[32] = {0};

    chemfiles::scan("3, 4.2 baz", "%i, %lf %31s", &i, &d, s);
    CHECK(i == 3);
    CHECK(d == 4.2);
    CHECK(std::string(s) == "baz");

    CHECK_THROWS_AS(chemfiles::scan("3, 4.2", "%i, %lf %31s", &i, &d, s), chemfiles::Error);
}
