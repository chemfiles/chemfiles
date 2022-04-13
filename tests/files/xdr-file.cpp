// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/XDRFile.hpp"
#include "helpers.hpp"
#include <vector>
using namespace chemfiles;

static void write_xdr_file(XDRFile& file) {
    file.write_int(-123);
    file.write_uint(123);
    file.write_float(-4.567f);
    const std::vector<float> arr = {1.234, -5.123, 100.232};
    file.write_float_array(arr);
    file.write_gmx_string("Hello!"); // needs 2B padding
    file.write_gmx_compressed_floats(arr, 1000.0);
}

TEST_CASE("XDR files") {
    SECTION("read") {
        XDRFile file("data/misc/xdr.bin", File::READ);
        CHECK(file.file_size() == 112);

        CHECK(file.read_int() == -123);
        CHECK(file.read_uint() == 123);
        CHECK(file.read_double() == 5.678);
        CHECK(file.read_float() == -4.567f);

        std::vector<double> darr;
        darr.resize(3);
        file.read_double_array(darr);
        const std::vector<double> dexpected = {1.234, -6.234, 105.232};
        CHECK(darr == dexpected);

        std::vector<float> farr;
        farr.resize(3);
        file.read_float_array(farr);
        const std::vector<float> fexpected = {1.234, -5.123, 100.232};
        CHECK(farr == fexpected);
        farr = {0.0, 0.0, 0.0};

        CHECK(file.read_gmx_string() == "Hello!");
        CHECK(file.read_gmx_compressed_floats(farr) == 1000.0);
        for (size_t i = 0; i < 3; ++i) {
            CHECK(approx_eq(farr[i], fexpected[i], 1e-4));
        }
    }

    // clang-format off
    auto expected = std::vector<uint8_t> {
        0xff, 0xff, 0xff, 0x85,
        0x00, 0x00, 0x00, 0x7b,
        0xc0, 0x92, 0x24, 0xdd,
        0x3f, 0x9d, 0xf3, 0xb6, 0xc0, 0xa3, 0xef, 0x9e, 0x42, 0xc8, 0x76, 0xc9,
        0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06,
        'H', 'e', 'l', 'l', 'o', '!', 0x00, 0x00,
        0x44, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x04, 0xd2, 0xff, 0xff, 0xeb, 0xfd,
        0x00, 0x01, 0x87, 0x88, 0x00, 0x00, 0x04, 0xd2, 0xff, 0xff, 0xeb, 0xfd,
        0x00, 0x01, 0x87, 0x88, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x01,
        0x42, 0x00, 0x00, 0x00,
    };
    // clang-format on
    auto expected_2 = expected;
    expected_2.insert(expected_2.end(), expected.begin(), expected.end());

    SECTION("write") {
        auto filename = NamedTempPath(".bin");
        {
            XDRFile file(filename, File::WRITE);
            write_xdr_file(file);
            CHECK(file.file_size() == expected.size());
        }

        auto content = read_binary_file(filename);
        CHECK(content == expected);
    }

    SECTION("write and append") {
        auto filename = NamedTempPath(".bin");
        {
            XDRFile file(filename, File::WRITE);
            write_xdr_file(file);
            CHECK(file.file_size() == expected.size());
        }

        auto content = read_binary_file(filename);
        CHECK(content == expected);

        {
            XDRFile file(filename, File::Mode::APPEND);
            write_xdr_file(file);
            CHECK(file.file_size() == expected_2.size());
        }

        content = read_binary_file(filename);
        CHECK(content == expected_2);
    }

    SECTION("append") {
        auto filename = NamedTempPath(".bin");
        {
            XDRFile file(filename, File::WRITE);
            write_xdr_file(file);
            CHECK(file.file_size() == expected.size());
        }

        auto content = read_binary_file(filename);
        CHECK(content == expected);
    }
}
