// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;
#include <boost/asio.hpp>
namespace ip = boost::asio::ip;

#include "chemfiles/parse.hpp"
#include "chemfiles/Error.hpp"

TEST_CASE("String parsing") {
    SECTION("Double") {
        CHECK(chemfiles::parse<double>("12.5") == 12.5);
        CHECK(chemfiles::parse<double>("125") == 125);
        CHECK(chemfiles::parse<double>("-32") == -32);

        CHECK_THROWS_WITH(
            chemfiles::parse<double>(""),
            "can not convert '' to a double"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<double>("foo"),
            "can not convert 'foo' to a double"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<double>("1,2"),
            "can not convert '1,2' to a double"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<double>("3e456782"),
            "3e456782 is out of range for double"
        );
    }

    SECTION("long long") {
        CHECK(chemfiles::parse<long long>("125") == 125);
        CHECK(chemfiles::parse<long long>("-32") == -32);

        CHECK_THROWS_WITH(
            chemfiles::parse<long long>(""),
            "can not convert '' to a long long integer"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<long long>("foo"),
            "can not convert 'foo' to a long long integer"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<long long>("2.5"),
            "can not convert '2.5' to a long long integer"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<long long>("9223372036854775808"),
            "9223372036854775808 is out of range for long long integer"
        );
    }

    SECTION("size_t") {
        CHECK(chemfiles::parse<size_t>("125") == 125);

        CHECK_THROWS_WITH(
            chemfiles::parse<size_t>(""),
            "can not convert '' to a long long integer"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<size_t>("-32"),
            "invalid integer: should be positive, is -32"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<size_t>("foo"),
            "can not convert 'foo' to a long long integer"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<size_t>("2.5"),
            "can not convert '2.5' to a long long integer"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<size_t>("9223372036854775808"),
            "9223372036854775808 is out of range for long long integer"
        );
    }

    SECTION("Other integer types") {
        CHECK(chemfiles::parse<uint8_t>("125") == 125);
        CHECK_THROWS_WITH(
            chemfiles::parse<uint8_t>("265"),
            "265 is out of range for this type"
        );

        CHECK(chemfiles::parse<int8_t>("125") == 125);
        CHECK_THROWS_WITH(
            chemfiles::parse<int8_t>("160"),
            "160 is out of range for this type"
        );
    }

    SECTION("string") {
        CHECK(chemfiles::parse<std::string>("125") == "125");
        CHECK(chemfiles::parse<std::string>("foo bar") == "foo bar");

        CHECK_THROWS_WITH(
            chemfiles::parse<std::string>(""),
            "tried to read a string, got an empty value"
        );
    }
}

TEST_CASE("scan") {
    int i = 0;
    double d = 0;
    std::string s;

    auto count = chemfiles::scan("3 4.2 baz", i, d, s);
    CHECK((i == 3 && d == 4.2 && s == "baz"));
    CHECK(count == 9);

    count = chemfiles::scan("     \t 5 \n\n   8.3      foo", i, d, s);
    CHECK((i == 5 && d == 8.3 && s == "foo"));
    CHECK(count == 26);

    CHECK_THROWS_WITH(
        chemfiles::scan("3 4.2", i, d, s),
        "error while reading '3 4.2': tried to read 3 values, but there are only 2"
    );

    CHECK_THROWS_WITH(
        chemfiles::scan("4.2 4", i, d),
        "error while reading '4.2 4': can not convert '4.2' to a long long integer"
    );
}
