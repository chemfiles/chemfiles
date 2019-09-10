// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;
#include <boost/asio.hpp>
namespace ip = boost::asio::ip;

#include "chemfiles/parse.hpp"
#include "chemfiles/Error.hpp"

static bool relative_eq(double a, double b) {
    return fabs((b - a) / a) < 1e-14;
}

TEST_CASE("String parsing") {
    SECTION("Double") {
        CHECK(chemfiles::parse<double>("12.5") == 12.5);
        CHECK(chemfiles::parse<double>("-32") == -32.0);

        CHECK(chemfiles::parse<double>(".1") == .1);
        CHECK(chemfiles::parse<double>(".1e2") == .1e2);
        CHECK(chemfiles::parse<double>("1.2e3") == 1.2e3);
        CHECK(chemfiles::parse<double>("-1.2e3") == -1.2e3);
        CHECK(chemfiles::parse<double>("+1.2e3") == +1.2e3);
        CHECK(chemfiles::parse<double>("-1.2e0") == -1.2e0);
        CHECK(chemfiles::parse<double>(".1e0") == .1e0);
        CHECK(chemfiles::parse<double>(".1e3") == .1e3);
        CHECK(chemfiles::parse<double>(".1e-3") == .1e-3);
        CHECK(chemfiles::parse<double>("1.768e00000000000000") == 1.768e0);
        CHECK(chemfiles::parse<double>("1.3e0") == 1.3e0);
        CHECK(chemfiles::parse<double>("3.") == 3.);
        CHECK(chemfiles::parse<double>("3.e2") == 3.e2);

        CHECK(chemfiles::parse<double>("    \t2.3") == 2.3);
        CHECK(chemfiles::parse<double>("2.3\n  ") == 2.3);
        CHECK(chemfiles::parse<double>("    2.3  \n") == 2.3);

        CHECK(chemfiles::parse<double>("0.0") == 0.0);
        CHECK(chemfiles::parse<double>("0") == 0.0);
        CHECK(chemfiles::parse<double>("+0.0") == 0.0);
        CHECK(chemfiles::parse<double>("-0.0") == 0.0);

        // Some float are not parsed exactly, but up to a 1e-14 RELATIVE error
        // which is good enough for our purposes
        CHECK(relative_eq(chemfiles::parse<double>("1.97576e0"), 1.97576e0));
        CHECK(relative_eq(chemfiles::parse<double>("2.27e-308"), 2.27e-308));
        CHECK(relative_eq(chemfiles::parse<double>("1.15507e-173"), 1.15507e-173));
        CHECK(relative_eq(
            chemfiles::parse<double>("0.000000000000000000000000000000000000783475"),
            0.000000000000000000000000000000000000783475
        ));

        CHECK_THROWS_WITH(
            chemfiles::parse<double>(""),
            "can not parse a double from an empty string"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<double>("3e456782"),
            "3e456782 is out of range for double"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<double>("3.e"),
            "missing exponent in '3.e'"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<double>(".e1"),
            "can not parse '.e1' as a double"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<double>("."),
            "can not parse '.' as a double"
        );

        auto BAD = {
            "nan", "NaN", "nan(0xfff)",
            "inf", "-inf", "INF", "infinity",
            "1,2", "foo", "2.3foo", "2.3 bar",
        };
        for (auto bad: BAD) {
            CHECK_THROWS_WITH(
                chemfiles::parse<double>(bad),
                "can not parse '" + std::string(bad) + "' as a double"
            );
        }
    }

    SECTION("int64_t") {
        CHECK(chemfiles::parse<int64_t>("125") == 125);
        CHECK(chemfiles::parse<int64_t>("-32") == -32);
        CHECK(chemfiles::parse<int64_t>("563940907") == 563940907);
        CHECK(chemfiles::parse<int64_t>("-125673024611") == -125673024611);

        CHECK(chemfiles::parse<int64_t>("00000000000125") == 125);
        CHECK(chemfiles::parse<int64_t>("-00000000000032") == -32);

        CHECK(chemfiles::parse<int64_t>("0") == 0);
        CHECK(chemfiles::parse<int64_t>("-0") == 0);

        CHECK(chemfiles::parse<int64_t>("    \t-23") == -23);
        CHECK(chemfiles::parse<int64_t>("-23\n  ") == -23);
        CHECK(chemfiles::parse<int64_t>("    -23  \n") == -23);

        // min/max int64_t value
        CHECK(chemfiles::parse<int64_t>("9223372036854775807") == std::numeric_limits<int64_t>::max());
        CHECK(chemfiles::parse<int64_t>("-9223372036854775808") == std::numeric_limits<int64_t>::min());

        CHECK_THROWS_WITH(
            chemfiles::parse<int64_t>(""),
            "can not parse an integer from an empty string"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<int64_t>("9223372036854775808"),
            "9223372036854775808 is out of range for 64-bit integer"
        );

        auto BAD = {"foo", "5673bar", "5673  bar", "2.5"};
        for (auto bad: BAD) {
            CHECK_THROWS_WITH(
                chemfiles::parse<int64_t>(bad),
                "can not parse '" + std::string(bad) + "' as an integer"
            );
        }
    }

    SECTION("uint64_t") {
        CHECK(chemfiles::parse<uint64_t>("125") == 125);
        CHECK(chemfiles::parse<uint64_t>("0") == 0);
        CHECK(chemfiles::parse<uint64_t>("456720463") == 456720463);
        CHECK(chemfiles::parse<uint64_t>("0000000000000125") == 125);
        // max uint64_t value
        CHECK(chemfiles::parse<uint64_t>("18446744073709551615") == 18446744073709551615ull);

        CHECK(chemfiles::parse<int64_t>("    \t32") == 32);
        CHECK(chemfiles::parse<int64_t>("32\n  ") == 32);
        CHECK(chemfiles::parse<int64_t>("    32  \n") == 32);

        CHECK_THROWS_WITH(
            chemfiles::parse<uint64_t>(""),
            "can not parse an integer from an empty string"
        );
        CHECK_THROWS_WITH(
            chemfiles::parse<uint64_t>("18446744073709551616"),
            "18446744073709551616 is out of range for 64-bit unsigned integer"
        );

        auto BAD = {
            "foo", "5673bar", "5673  bar", "2.5", "-32"
        };
        for (auto bad: BAD) {
            CHECK_THROWS_WITH(
                chemfiles::parse<uint64_t>(bad),
                "can not parse '" + std::string(bad) + "' as a positive integer"
            );
        }
    }

    SECTION("Other integer types") {
        CHECK(chemfiles::parse<unsigned>("125") == 125);
        CHECK_THROWS_WITH(
            chemfiles::parse<unsigned>("-32"),
            "can not parse '-32' as a positive integer"
        );

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
        "error while reading '4.2 4': can not parse '4.2' as an integer"
    );
}
