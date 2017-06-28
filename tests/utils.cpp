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
    auto expected = std::vector<std::string>{"bla", "bla  bla, jk", "fiuks"};
    CHECK(chemfiles::split("bla:bla  bla, jk:fiuks", ':') == expected);

    expected = std::vector<std::string>{"", "", "bla  bla", " jk:fiuks"};
    CHECK(chemfiles::split(",,bla  bla, jk:fiuks", ',') == expected);
}

TEST_CASE("String to double") {
    CHECK(chemfiles::string2double("12.5") == 12.5);
    CHECK(chemfiles::string2longlong("125") == 125);
    CHECK(chemfiles::string2longlong("-32") == -32);

    CHECK_THROWS_AS(chemfiles::string2double("foo"), chemfiles::Error);
    CHECK_THROWS_AS(chemfiles::string2double("1,2"), chemfiles::Error);
    CHECK_THROWS_AS(chemfiles::string2double("3e456782"), chemfiles::Error);

    CHECK_THROWS_AS(chemfiles::string2longlong("foo"), chemfiles::Error);
    CHECK_THROWS_AS(chemfiles::string2longlong("2.5"), chemfiles::Error);
    CHECK_THROWS_AS(chemfiles::string2longlong("9223372036854775808"), chemfiles::Error);
}
