// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include "catch.hpp"
#include "helpers.hpp"

#include "chemfiles/files/BinaryFile.hpp"
#include "chemfiles/Error.hpp"
using namespace chemfiles;

#define CHECK_BINARY_ARRAY(_array_)  \
    CHECK(_array_[0] == 0);   \
    CHECK(_array_[1] == 1);   \
    CHECK(_array_[2] == 2);   \
    CHECK(_array_[3] == 3);   \
    CHECK(_array_[4] == 4);   \
    CHECK(_array_[5] == 5);   \
    CHECK(_array_[6] == 6);   \
    CHECK(_array_[7] == 7);   \
    CHECK(_array_[8] == 8);   \
    CHECK(_array_[9] == 9)

static void check_read_binary_file(BinaryFile& file) {
    char buffer[8] = {0};
    file.read_char(buffer, 4);
    CHECK(buffer == std::string("DATA"));

    CHECK(file.read_single_char() == '2');

    CHECK(file.read_single_i16() == -42);
    CHECK(file.read_single_u16() == 42);

    CHECK(file.read_single_i32() == -573);
    CHECK(file.read_single_u32() == 573);

    CHECK(file.read_single_i64() == -123456);
    CHECK(file.read_single_u64() == 123456);

    CHECK(file.read_single_f32() == 33.3f);
    CHECK(file.read_single_f64() == -55.8);

    auto size = file.read_single_i32();
    CHECK(size == 5);
    file.read_char(buffer, static_cast<size_t>(size));
    CHECK(buffer == std::string("hello"));

    int16_t i16[10];
    file.read_i16(i16, 10);
    CHECK_BINARY_ARRAY(i16);

    uint16_t u16[10];
    file.read_u16(u16, 10);
    CHECK_BINARY_ARRAY(u16);

    int32_t i32[10];
    file.read_i32(i32, 10);
    CHECK_BINARY_ARRAY(i32);

    uint32_t u32[10];
    file.read_u32(u32, 10);
    CHECK_BINARY_ARRAY(u32);

    int64_t i64[10];
    file.read_i64(i64, 10);
    CHECK_BINARY_ARRAY(i64);

    uint64_t u64[10];
    file.read_u64(u64, 10);
    CHECK_BINARY_ARRAY(u64);

    float f32[10];
    file.read_f32(f32, 10);
    CHECK_BINARY_ARRAY(f32);

    double f64[10];
    file.read_f64(f64, 10);
    CHECK_BINARY_ARRAY(f64);

    // skip back to the beginning of all the arrays
    file.seek(file.file_size() - 400L);

    std::vector<int16_t> vec_i16(10);
    file.read_i16(vec_i16);
    CHECK_BINARY_ARRAY(vec_i16);

    std::vector<uint16_t> vec_u16(10);
    file.read_u16(vec_u16);
    CHECK_BINARY_ARRAY(vec_u16);

    std::vector<int32_t> vec_i32(10);
    file.read_i32(vec_i32);
    CHECK_BINARY_ARRAY(vec_i32);

    std::vector<uint32_t> vec_u32(10);
    file.read_u32(vec_u32);
    CHECK_BINARY_ARRAY(vec_u32);

    std::vector<int64_t> vec_i64(10);
    file.read_i64(vec_i64);
    CHECK_BINARY_ARRAY(vec_i64);

    std::vector<uint64_t> vec_u64(10);
    file.read_u64(vec_u64);
    CHECK_BINARY_ARRAY(vec_u64);

    std::vector<float> vec_f32(10);
    file.read_f32(vec_f32);
    CHECK_BINARY_ARRAY(vec_f32);

    std::vector<double> vec_f64(10);
    file.read_f64(vec_f64);
    CHECK_BINARY_ARRAY(vec_f64);

    CHECK_THROWS_AS(file.read_single_char(), FileError);
}

static void check_read_skip_binary_file(BinaryFile& file) {
    file.seek(0L);
    file.skip(7L);
    CHECK(file.read_single_u16() == 42);
    file.skip(8L);
    CHECK(file.read_single_i64() == -123456);
    file.skip(341L); // -88B (10*f64 + 2*f32) from end
    CHECK(file.read_single_f32() == 8.0f);
}

TEST_CASE("Read binary files") {
    SECTION("big endian") {
        auto file = BigEndianFile("data/misc/big-endian.dat", File::Mode::READ);
        CHECK(file.file_size() == 454);
        check_read_binary_file(file);
        check_read_skip_binary_file(file);
    }

    SECTION("little endian") {
        auto file = LittleEndianFile("data/misc/little-endian.dat", File::Mode::READ);
        CHECK(file.file_size() == 454);
        check_read_binary_file(file);
        check_read_skip_binary_file(file);
    }
}

static void write_binary_file(BinaryFile& file) {
    file.write_char("ABCD", 4);
    file.write_single_i16(-42);
    file.write_single_u16(42);

    file.write_single_i32(-573);
    file.write_single_u32(573);

    file.write_single_i64(-123456);
    file.write_single_u64(123456);

    file.write_single_f32(33.3f);
    file.write_single_f64(-55.8);

    const std::vector<char> vec_char = {'E', 'F', 'G'};
    file.write_char(vec_char);

    const std::vector<int8_t> vec_i8 = {-117, 122, 27};
    file.write_i8(vec_i8);
    const std::vector<uint8_t> vec_u8 = {11, 250, 27};
    file.write_u8(vec_u8);

    const std::vector<int16_t> vec_i16 = {-32757, 32762, 27};
    file.write_i16(vec_i16);
    const std::vector<uint16_t> vec_u16 = {11, 65530, 27};
    file.write_u16(vec_u16);

    const std::vector<int32_t> vec_i32 = {-2147483637, 2147483642, 27};
    file.write_i32(vec_i32);
    const std::vector<uint32_t> vec_u32 = {11, 4294967290, 27};
    file.write_u32(vec_u32);

    const std::vector<int64_t> vec_i64 = {-9223372036854775797L, 9223372036854775802L, 27L};
    file.write_i64(vec_i64);
    const std::vector<uint64_t> vec_u64 = {11U, 18446744073709551610U, 27U};
    file.write_u64(vec_u64);

    const std::vector<float> vec_f32 = {1.234, -5.123, 100.232};
    file.write_f32(vec_f32);
    const std::vector<double> vec_f64 = {1.234, -5.123, 100.232};
    file.write_f64(vec_f64);
}

TEST_CASE("Write binary files") {
    SECTION("big endian") {
        auto expected = std::vector<uint8_t> {
            'A', 'B', 'C', 'D',
            0xff, 0xd6,
            0x0,  0x2a,
            0xff, 0xff, 0xfd, 0xc3,
            0x0,  0x0,  0x2,  0x3d,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1d, 0xc0,
            0x0,  0x0,  0x0,  0x0,  0x0,  0x1,  0xe2, 0x40,
            0x42, 0x5, 0x33, 0x33,
            0xc0, 0x4b, 0xe6, 0x66, 0x66, 0x66, 0x66, 0x66,
            'E', 'F', 'G',
            0x8b, 0x7a, 0x1b,
            0x0b, 0xfa, 0x1b,
            0x80, 0x0b, 0x7f, 0xfa, 0x00, 0x1b,
            0x00, 0x0b, 0xff, 0xfa, 0x00, 0x1b,
            // i32 vector
            0x80, 0x00, 0x00, 0x0b,
            0x7f, 0xff, 0xff, 0xfa,
            0x00, 0x00, 0x00, 0x1b,
            // u32 vector
            0x00, 0x00, 0x00, 0x0b,
            0xff, 0xff, 0xff, 0xfa,
            0x00, 0x00, 0x00, 0x1b,
            // i64 vector
            0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b,
            0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b,
            // u64 vector
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b,
            // float vector
            0x3f, 0x9d, 0xf3, 0xb6,
            0xc0, 0xa3, 0xef, 0x9e,
            0x42, 0xc8, 0x76, 0xc9,
            // double vector
            0x3f, 0xf3, 0xbe, 0x76, 0xc8, 0xb4, 0x39, 0x58,
            0xc0, 0x14, 0x7d, 0xf3, 0xb6, 0x45, 0xa1, 0xcb,
            0x40, 0x59, 0x0e, 0xd9, 0x16, 0x87, 0x2b, 0x02,
        };
        auto expected_2 = expected;
        expected_2.insert(expected_2.end(), expected.begin(), expected.end());

        SECTION("write to file") {
            auto filename = NamedTempPath(".data");
            {
                auto file = BigEndianFile(filename, File::Mode::WRITE);
                write_binary_file(file);
                CHECK(file.file_size() == expected.size());
            }

            auto content = read_binary_file(filename);
            CHECK(content == expected);
        }

        SECTION("write and append") {
            auto filename = NamedTempPath(".data");
            {
                auto file = BigEndianFile(filename, File::Mode::WRITE);
                write_binary_file(file);
                CHECK(file.file_size() == expected.size());
            }

            auto content = read_binary_file(filename);
            CHECK(content == expected);

            {
                auto file = BigEndianFile(filename, File::Mode::APPEND);
                write_binary_file(file);
                CHECK(file.file_size() == expected_2.size());
            }

            content = read_binary_file(filename);
            CHECK(content == expected_2);
        }

        SECTION("append") {
            auto filename = NamedTempPath(".data");
            {
                auto file = BigEndianFile(filename, File::Mode::APPEND);
                write_binary_file(file);
                CHECK(file.file_size() == expected.size());
            }

            auto content = read_binary_file(filename);
            CHECK(content == expected);
        }
    }

    SECTION("little endian") {
        auto expected = std::vector<uint8_t> {
            'A', 'B', 'C', 'D',
            0xd6, 0xff,
            0x2a, 0x0,
            0xc3, 0xfd, 0xff, 0xff,
            0x3d, 0x2,  0x0,  0x0,
            0xc0, 0x1d, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x40, 0xe2, 0x1,  0x0,  0x0,  0x0,  0x0,  0x0,
            0x33, 0x33, 0x5,  0x42,
            0x66, 0x66, 0x66, 0x66, 0x66, 0xe6, 0x4b, 0xc0,
            'E', 'F', 'G',
            0x8b, 0x7a, 0x1b,
            0x0b, 0xfa, 0x1b,
            0x0b, 0x80, 0xfa, 0x7f, 0x1b, 0x00,
            0x0b, 0x00, 0xfa, 0xff, 0x1b, 0x00,
            // i32 vector
            0x0b, 0x00, 0x00, 0x80,
            0xfa, 0xff, 0xff, 0x7f,
            0x1b, 0x00, 0x00, 0x00,
            // u32 vector
            0x0b, 0x00, 0x00, 0x00,
            0xfa, 0xff, 0xff, 0xff,
            0x1b, 0x00, 0x00, 0x00,
            // i64 vector
            0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
            0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
            0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            // u64 vector
            0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            // float vector
            0xb6, 0xf3, 0x9d, 0x3f,
            0x9e, 0xef, 0xa3, 0xc0,
            0xc9, 0x76, 0xc8, 0x42,
            // double vector
            0x58, 0x39, 0xb4, 0xc8, 0x76, 0xbe, 0xf3, 0x3f,
            0xcb, 0xa1, 0x45, 0xb6, 0xf3, 0x7d, 0x14, 0xc0,
            0x02, 0x2b, 0x87, 0x16, 0xd9, 0x0e, 0x59, 0x40,
        };
        auto expected_2 = expected;
        expected_2.insert(expected_2.end(), expected.begin(), expected.end());

        SECTION("write to file") {
            auto filename = NamedTempPath(".data");
            {
                auto file = LittleEndianFile(filename, File::Mode::WRITE);
                write_binary_file(file);
                CHECK(file.file_size() == expected.size());
            }

            auto content = read_binary_file(filename);
            CHECK(content == expected);
        }

        SECTION("write and append") {
            auto filename = NamedTempPath(".data");
            {
                auto file = LittleEndianFile(filename, File::Mode::WRITE);
                write_binary_file(file);
                CHECK(file.file_size() == expected.size());
            }

            auto content = read_binary_file(filename);
            CHECK(content == expected);

            {
                auto file = LittleEndianFile(filename, File::Mode::APPEND);
                write_binary_file(file);
                CHECK(file.file_size() == expected_2.size());
            }

            content = read_binary_file(filename);
            CHECK(content == expected_2);
        }

        SECTION("append") {
            auto filename = NamedTempPath(".data");
            {
                auto file = LittleEndianFile(filename, File::Mode::APPEND);
                write_binary_file(file);
                CHECK(file.file_size() == expected.size());
            }

            auto content = read_binary_file(filename);
            CHECK(content == expected);
        }
    }
}
