// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>

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
            "missing exponent in '3.e' to read a double"
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
        "error while reading '3 4.2': expected 3 values, found 2"
    );

    CHECK_THROWS_WITH(
        chemfiles::scan("4.2 4", i, d),
        "error while reading '4.2 4': can not parse '4.2' as an integer"
    );
}

static void recycle(uint64_t width, int64_t value, const std::string& hybrid) {
    CHECK(chemfiles::encode_hybrid36(width, value) == hybrid);
    CHECK(chemfiles::decode_hybrid36(width, hybrid) == value);
}

TEST_CASE("hybrid encode and decode") {
    CHECK(chemfiles::decode_hybrid36(4, "    ") == 0);
    CHECK(chemfiles::decode_hybrid36(4, "  -0") == 0);
    recycle(4, -999, "-999");
    recycle(4, -78, "-78");
    recycle(4, -6, "-6");
    recycle(4, 0, "0");
    recycle(4, 9999, "9999");
    recycle(4, 10000, "A000");
    recycle(4, 10001, "A001");
    recycle(4, 10002, "A002");
    recycle(4, 10003, "A003");
    recycle(4, 10004, "A004");
    recycle(4, 10005, "A005");
    recycle(4, 10006, "A006");
    recycle(4, 10007, "A007");
    recycle(4, 10008, "A008");
    recycle(4, 10009, "A009");
    recycle(4, 10010, "A00A");
    recycle(4, 10011, "A00B");
    recycle(4, 10012, "A00C");
    recycle(4, 10013, "A00D");
    recycle(4, 10014, "A00E");
    recycle(4, 10015, "A00F");
    recycle(4, 10016, "A00G");
    recycle(4, 10017, "A00H");
    recycle(4, 10018, "A00I");
    recycle(4, 10019, "A00J");
    recycle(4, 10020, "A00K");
    recycle(4, 10021, "A00L");
    recycle(4, 10022, "A00M");
    recycle(4, 10023, "A00N");
    recycle(4, 10024, "A00O");
    recycle(4, 10025, "A00P");
    recycle(4, 10026, "A00Q");
    recycle(4, 10027, "A00R");
    recycle(4, 10028, "A00S");
    recycle(4, 10029, "A00T");
    recycle(4, 10030, "A00U");
    recycle(4, 10031, "A00V");
    recycle(4, 10032, "A00W");
    recycle(4, 10033, "A00X");
    recycle(4, 10034, "A00Y");
    recycle(4, 10035, "A00Z");
    recycle(4, 10036, "A010");
    recycle(4, 10046, "A01A");
    recycle(4, 10071, "A01Z");
    recycle(4, 10072, "A020");
    recycle(4, 10000 + 36 * 36 - 1, "A0ZZ");
    recycle(4, 10000 + 36 * 36, "A100");
    recycle(4, 10000 + 36 * 36 * 36 - 1, "AZZZ");
    recycle(4, 10000 + 36 * 36 * 36, "B000");
    recycle(4, 10000 + 26 * 36 * 36 * 36 - 1, "ZZZZ");
    recycle(4, 10000 + 26 * 36 * 36 * 36, "a000");
    recycle(4, 10000 + 26 * 36 * 36 * 36 + 35, "a00z");
    recycle(4, 10000 + 26 * 36 * 36 * 36 + 36, "a010");
    recycle(4, 10000 + 26 * 36 * 36 * 36 + 36 * 36 - 1, "a0zz");
    recycle(4, 10000 + 26 * 36 * 36 * 36 + 36 * 36, "a100");
    recycle(4, 10000 + 26 * 36 * 36 * 36 + 36 * 36 * 36 - 1, "azzz");
    recycle(4, 10000 + 26 * 36 * 36 * 36 + 36 * 36 * 36, "b000");
    recycle(4, 10000 + 2 * 26 * 36 * 36 * 36 - 1, "zzzz");

    CHECK(chemfiles::decode_hybrid36(5, "    ") == 0);
    CHECK(chemfiles::decode_hybrid36(5, "  -0") == 0);
    recycle(5, -9999, "-9999");
    recycle(5, -123, "-123");
    recycle(5, -45, "-45");
    recycle(5, -6, "-6");
    recycle(5, 0, "0");
    recycle(5, 12, "12");
    recycle(5, 345, "345");
    recycle(5, 6789, "6789");
    recycle(5, 99999, "99999");
    recycle(5, 100000, "A0000");
    recycle(5, 100010, "A000A");
    recycle(5, 100035, "A000Z");
    recycle(5, 100036, "A0010");
    recycle(5, 100046, "A001A");
    recycle(5, 100071, "A001Z");
    recycle(5, 100072, "A0020");
    recycle(5, 100000 + 36 * 36 - 1, "A00ZZ");
    recycle(5, 100000 + 36 * 36, "A0100");
    recycle(5, 100000 + 36 * 36 * 36 - 1, "A0ZZZ");
    recycle(5, 100000 + 36 * 36 * 36, "A1000");
    recycle(5, 100000 + 36 * 36 * 36 * 36 - 1, "AZZZZ");
    recycle(5, 100000 + 36 * 36 * 36 * 36, "B0000");
    recycle(5, 100000 + 2 * 36 * 36 * 36 * 36, "C0000");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 - 1, "ZZZZZ");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36, "a0000");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 + 36 - 1, "a000z");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 + 36, "a0010");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 + 36 * 36 - 1, "a00zz");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 + 36 * 36, "a0100");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 + 36 * 36 * 36 - 1, "a0zzz");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 + 36 * 36 * 36, "a1000");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 + 36 * 36 * 36 * 36 - 1, "azzzz");
    recycle(5, 100000 + 26 * 36 * 36 * 36 * 36 + 36 * 36 * 36 * 36, "b0000");
    recycle(5, 100000 + 2 * 26 * 36 * 36 * 36 * 36 - 1, "zzzzz");

    CHECK(chemfiles::encode_hybrid36(4, -99999) == "****");
    CHECK(chemfiles::encode_hybrid36(4, 9999999) == "****");

    CHECK_THROWS_WITH(
        chemfiles::decode_hybrid36(5, "*0000"),
        "the value '*0000' is not a valid hybrid 36 number"
    );

    CHECK_THROWS_WITH(
        chemfiles::decode_hybrid36(5, "A*000"),
        "the value 'A*000' is not a valid hybrid 36 number"
    );

    CHECK_THROWS_WITH(
        chemfiles::decode_hybrid36(5, "a*000"),
        "the value 'a*000' is not a valid hybrid 36 number"
    );

    CHECK_THROWS_WITH(
        chemfiles::decode_hybrid36(2, "12345"),
        "the length of '12345' is greater than the width '2', this is a bug in chemfiles"
    );
}
