// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles/selections/parser.hpp"
#include "chemfiles/selections/expr.hpp"

using namespace chemfiles;

TEST_CASE("NumericValues") {
    SECTION("empty") {
        auto vec = NumericValues();
        CHECK(vec.size() == 0);
        CHECK(vec.capacity() == 1);

        vec.push_back(42);
        CHECK(vec.size() == 1);
        CHECK(vec.capacity() == 1);
        CHECK(vec[0] == 42);
    }

    SECTION("invalid value") {
        auto vec = NumericValues();
        CHECK_THROWS_WITH(vec.push_back(static_cast<double>(INFINITY)), "invalid value +inf as first value of NumericValues");

        vec.push_back(static_cast<double>(-INFINITY));
        CHECK(vec.size() == 1);
        CHECK(vec.capacity() == 1);
        CHECK(vec[0] == static_cast<double>(-INFINITY));
    }

    SECTION("contains 1 value") {
        auto vec = NumericValues(3);
        CHECK(vec.size() == 1);
        CHECK(vec.capacity() == 1);
        CHECK(vec[0] == 3);

        auto count = 0;
        auto sum = 0.0;
        for (const auto value: vec) {
            sum += value;
            count += 1;
        }
        CHECK(count == 1);
        CHECK(sum == 3.0);
    }

    SECTION("multiple values") {
        auto vec = NumericValues();
        vec.reserve(32);
        CHECK(vec.size() == 0);
        CHECK(vec.capacity() == 32);

        for (size_t i=0; i<22; i++) {
            vec.push_back(static_cast<double>(i));
            CHECK(vec.size() == i + 1);
        }

        for (size_t i=0; i<22; i++) {
            CHECK(vec[i] == static_cast<double>(i));
        }

        CHECK(vec.size() == 22);
        CHECK(vec.capacity() > 22);

        auto count = 0;
        auto sum = 0.0;
        for (const auto value: vec) {
            sum += value;
            count += 1;
        }
        CHECK(count == 22);
        CHECK(sum == 231.0);
    }
}
