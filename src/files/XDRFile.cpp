// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <climits>
#include <string>
#include <vector>

#include "chemfiles/files/XDRFile.hpp"

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/span.hpp"
#include "chemfiles/warnings.hpp"

using namespace chemfiles;

XDRFile::XDRFile(std::string path, File::Mode mode) : BigEndianFile(std::move(path), mode) {}

void XDRFile::read_opaque(std::vector<char>& data) {
    const uint32_t count = read_uint();
    const uint32_t num_filler = (4 - (count % 4)) % 4;
    data.resize(static_cast<size_t>(count + num_filler));
    read_char(data.data(), count + num_filler);
    data.resize(count);
}

void XDRFile::write_opaque(const char* data, uint32_t count) {
    write_uint(count);
    write_char(data, count);
    const uint32_t num_filler = (4 - (count % 4)) % 4;
    const std::vector<char> filler(num_filler, 0);
    write_char(filler.data(), num_filler);
}

std::string XDRFile::read_gmx_string() {
    // lenght with null terminator
    const uint32_t len = read_uint();
    // next comes XDR string without terminator
    std::vector<char> buf;
    read_opaque(buf);
    assert(len == buf.size() + 1);
    return std::string(buf.begin(), buf.end());
}

void XDRFile::write_gmx_string(const std::string& value) {
    // lenght with null terminator
    const uint32_t len = static_cast<uint32_t>(value.size() + 1);
    write_uint(len);
    // next comes XDR string without terminator
    write_opaque(value.c_str(), len - 1);
}

/***** from xdrfile *****/

/* Internal support routines for reading/writing compressed coordinates
 * sizeofint - calculate smallest number of bits necessary
 * to represent a certain integer.
 */
static unsigned int sizeofint(unsigned int size) {
    unsigned int num = 1;
    unsigned int num_of_bits = 0;

    while (size >= num && num_of_bits < 32) {
        num_of_bits++;
        num <<= 1;
    }
    return num_of_bits;
}

/*
 * sizeofints - calculate 'bitsize' of compressed ints
 *
 * Given a number of small unsigned integers and the maximum value
 * return the number of bits needed to read or write them with the
 * routines encodeints/decodeints. You need this parameter when
 * calling those routines.
 * (However, in some cases we can just use the variable 'smallidx'
 * which is the exact number of bits, and them we dont need to call
 * this routine).
 */
static unsigned int sizeofints(unsigned int num_of_ints, unsigned int sizes[]) {
    unsigned int num_of_bytes = 1;
    unsigned int bytes[32];
    bytes[0] = 1;
    unsigned int num_of_bits = 0;
    for (unsigned int i = 0; i < num_of_ints; i++) {
        unsigned int tmp = 0;
        unsigned int bytecnt;
        for (bytecnt = 0; bytecnt < num_of_bytes; bytecnt++) {
            tmp = bytes[bytecnt] * sizes[i] + tmp;
            bytes[bytecnt] = tmp & 0xff;
            tmp >>= 8;
        }
        while (tmp != 0) {
            bytes[bytecnt++] = tmp & 0xff;
            tmp >>= 8;
        }
        num_of_bytes = bytecnt;
    }
    unsigned int num = 1;
    num_of_bytes--;
    while (bytes[num_of_bytes] >= num) {
        num_of_bits++;
        num *= 2;
    }
    return num_of_bits + num_of_bytes * 8;
}

struct DecodeState {
    unsigned int count;
    unsigned int lastbits;
    unsigned int lastbyte;
};

/*
 * encodebits - encode num into buf using the specified number of bits
 *
 * This routines appends the value of num to the bits already present in
 * the array buf. You need to give it the number of bits to use and you had
 * better make sure that this number of bits is enough to hold the value.
 * Num must also be positive.
 */
static void encodebits(std::vector<char>& buf, DecodeState& state, unsigned int num_of_bits,
                       unsigned int num) {
    unsigned int cnt = state.count;
    unsigned int lastbits = state.lastbits;
    unsigned int lastbyte = state.lastbyte;
    while (num_of_bits >= 8) {
        lastbyte = (lastbyte << 8) | (num >> (num_of_bits - 8));
        buf[cnt++] = static_cast<char>(lastbyte >> lastbits);
        num_of_bits -= 8;
    }
    if (num_of_bits > 0) {
        lastbyte = (lastbyte << num_of_bits) | num;
        lastbits += num_of_bits;
        if (lastbits >= 8) {
            lastbits -= 8;
            buf[cnt++] = static_cast<char>(lastbyte >> lastbits);
        }
    }
    state.count = cnt;
    state.lastbits = lastbits;
    state.lastbyte = lastbyte;
    if (lastbits > 0) {
        buf[cnt] = static_cast<char>(lastbyte << (8 - lastbits));
    }
}

/*
 * encodeints - encode a small set of small integers in compressed format
 *
 * This routine is used internally by xdr3dfcoord, to encode a set of
 * small integers to the buffer for writing to a file.
 * Multiplication with fixed (specified maximum) sizes is used to get
 * to one big, multibyte integer. Allthough the routine could be
 * modified to handle sizes bigger than 16777216, or more than just
 * a few integers, this is not done because the gain in compression
 * isn't worth the effort. Note that overflowing the multiplication
 * or the byte buffer (32 bytes) is unchecked and whould cause bad results.
 * THese things are checked in the calling routines, so make sure not
 * to remove those checks...
 */

static void encodeints(std::vector<char>& buf, DecodeState& state, unsigned int num_of_ints,
                       unsigned int num_of_bits, unsigned int sizes[], unsigned int nums[]) {
    unsigned int tmp = nums[0];
    unsigned int num_of_bytes = 0;
    unsigned int bytes[32];
    do {
        bytes[num_of_bytes++] = tmp & 0xff;
        tmp >>= 8;
    } while (tmp != 0);

    for (unsigned int i = 1; i < num_of_ints; i++) {
        if (nums[i] >= sizes[i]) {
            throw file_error("major breakdown in encodeints - num {} doesn't match size {}",
                             nums[i], sizes[i]);
        }
        /* use one step multiply */
        tmp = nums[i];
        unsigned int bytecnt;
        for (bytecnt = 0; bytecnt < num_of_bytes; bytecnt++) {
            tmp = bytes[bytecnt] * sizes[i] + tmp;
            bytes[bytecnt] = tmp & 0xff;
            tmp >>= 8;
        }
        while (tmp != 0) {
            bytes[bytecnt++] = tmp & 0xff;
            tmp >>= 8;
        }
        num_of_bytes = bytecnt;
    }
    if (num_of_bits >= num_of_bytes * 8) {
        for (unsigned int i = 0; i < num_of_bytes; i++) {
            encodebits(buf, state, 8, bytes[i]);
        }
        encodebits(buf, state, num_of_bits - num_of_bytes * 8, 0);
    } else {
        unsigned int i;
        for (i = 0; i < num_of_bytes - 1; i++) {
            encodebits(buf, state, 8, bytes[i]);
        }
        encodebits(buf, state, num_of_bits - (num_of_bytes - 1) * 8, bytes[i]);
    }
}

/*
 * decodebits - decode number from buf using specified number of bits
 *
 * Extract the number of bits from the array buf and construct an integer
 * from it. Return that value.
 *
 */

static unsigned int decodebits(const std::vector<char>& buf, DecodeState& state,
                               unsigned int num_of_bits) {
    unsigned int mask = static_cast<unsigned int>(1 << num_of_bits) - 1;

    unsigned int cnt = state.count;
    unsigned int lastbits = state.lastbits;
    unsigned int lastbyte = state.lastbyte;

    unsigned int num = 0;
    while (num_of_bits >= 8) {
        lastbyte = (lastbyte << 8) | static_cast<unsigned char>(buf[cnt++]);
        num |= (lastbyte >> lastbits) << (num_of_bits - 8);
        num_of_bits -= 8;
    }
    if (num_of_bits > 0) {
        if (lastbits < num_of_bits) {
            lastbits += 8;
            lastbyte = (lastbyte << 8) | static_cast<unsigned char>(buf[cnt++]);
        }
        lastbits -= num_of_bits;
        num |= (lastbyte >> lastbits) & mask;
    }
    num &= mask;
    state.count = cnt;
    state.lastbits = lastbits;
    state.lastbyte = lastbyte;
    return num;
}

/*
 * decodeints - decode 'small' integers from the buf array
 *
 * This routine is the inverse from encodeints() and decodes the small integers
 * written to buf by calculating the remainder and doing divisions with
 * the given sizes[]. You need to specify the total number of bits to be
 * used from buf in num_of_bits.
 *
 */

static void decodeints(const std::vector<char>& buf, DecodeState& state, unsigned int num_of_ints,
                       unsigned int num_of_bits, unsigned int sizes[3], span<int> nums) {
    unsigned int bytes[32];
    bytes[1] = bytes[2] = bytes[3] = 0;
    unsigned int num_of_bytes = 0;
    while (num_of_bits > 8) {
        bytes[num_of_bytes++] = decodebits(buf, state, 8);
        num_of_bits -= 8;
    }
    if (num_of_bits > 0) {
        bytes[num_of_bytes++] = decodebits(buf, state, num_of_bits);
    }
    for (unsigned int i = num_of_ints - 1; i > 0; i--) {
        unsigned int num = 0;
        for (int j = static_cast<int>(num_of_bytes - 1); j >= 0; j--) {
            num = (num << 8) | bytes[j];
            unsigned int p = num / sizes[i];
            bytes[j] = p;
            num = num - p * sizes[i];
        }
        nums[i] = static_cast<int>(num);
    }
    nums[0] = static_cast<int>(bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24));
}

static unsigned int calc_sizeint(const int minint[3], const int maxint[3], unsigned int sizeint[3],
                                 unsigned int bitsizeint[3]) {
    sizeint[0] = static_cast<unsigned int>(maxint[0] - minint[0]) + 1;
    sizeint[1] = static_cast<unsigned int>(maxint[1] - minint[1]) + 1;
    sizeint[2] = static_cast<unsigned int>(maxint[2] - minint[2]) + 1;

    bitsizeint[0] = bitsizeint[1] = bitsizeint[2] = 0;
    // check if one of the sizes is to big to be multiplied
    if ((sizeint[0] | sizeint[1] | sizeint[2]) > 0xffffff) {
        bitsizeint[0] = sizeofint(sizeint[0]);
        bitsizeint[1] = sizeofint(sizeint[1]);
        bitsizeint[2] = sizeofint(sizeint[2]);
        return 0; // flag the use of large sizes
    } else {
        return sizeofints(3, sizeint);
    }
}

//clang-format off
static const int MAGICINTS[] = {
    0,        0,        0,       0,       0,       0,       0,       0,       0,       8,
    10,       12,       16,      20,      25,      32,      40,      50,      64,      80,
    101,      128,      161,     203,     256,     322,     406,     512,     645,     812,
    1024,     1290,     1625,    2048,    2580,    3250,    4096,    5060,    6501,    8192,
    10321,    13003,    16384,   20642,   26007,   32768,   41285,   52015,   65536,   82570,
    104031,   131072,   165140,  208063,  262144,  330280,  416127,  524287,  660561,  832255,
    1048576,  1321122,  1664510, 2097152, 2642245, 3329021, 4194304, 5284491, 6658042, 8388607,
    10568983, 13316085, 16777216};
//clang-format on
#define FIRSTIDX 9 // note that MAGICINTS[FIRSTIDX-1] == 0
#define LASTIDX (sizeof(MAGICINTS) / sizeof(*MAGICINTS))

/***** from xdrfile (end) *****/

float XDRFile::read_gmx_compressed_floats(std::vector<float>& data) {
    const float precision = read_float();
    const int minint[3] = {
        read_int(),
        read_int(),
        read_int(),
    };
    const int maxint[3] = {
        read_int(),
        read_int(),
        read_int(),
    };
    unsigned int smallidx = static_cast<unsigned int>(read_int());

    unsigned int sizeint[3], bitsizeint[3];
    const unsigned int bitsize = calc_sizeint(minint, maxint, sizeint, bitsizeint);

    unsigned int tmpidx = smallidx - 1;
    tmpidx = (FIRSTIDX > tmpidx) ? FIRSTIDX : tmpidx;

    int smaller = MAGICINTS[tmpidx] / 2;
    int smallnum = MAGICINTS[smallidx] / 2;
    unsigned int sizesmall[3];
    sizesmall[0] = sizesmall[1] = sizesmall[2] = static_cast<unsigned int>(MAGICINTS[smallidx]);

    read_opaque(compressed_data_);
    intbuf_.resize(data.size());

    assert(data.size() % 3 == 0 && "internal Error: invalid allocation size");
    const size_t natoms = data.size() / 3;

    DecodeState state = {0, 0, 0};
    int run = 0;
    int prevcoord[3];
    const float inv_precision = 1.0f / precision;
    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < natoms; ++read_idx) {
        auto thiscoord = span<int>(intbuf_.data() + read_idx * 3, 3);
        auto thiscoord_fl = span<float>(data.data() + write_idx * 3, 3);

        if (bitsize == 0) {
            thiscoord[0] = static_cast<int>(decodebits(compressed_data_, state, bitsizeint[0]));
            thiscoord[1] = static_cast<int>(decodebits(compressed_data_, state, bitsizeint[1]));
            thiscoord[2] = static_cast<int>(decodebits(compressed_data_, state, bitsizeint[2]));
        } else {
            decodeints(compressed_data_, state, 3, bitsize, sizeint, thiscoord);
        }

        thiscoord[0] += minint[0];
        thiscoord[1] += minint[1];
        thiscoord[2] += minint[2];

        prevcoord[0] = thiscoord[0];
        prevcoord[1] = thiscoord[1];
        prevcoord[2] = thiscoord[2];

        const unsigned int flag = decodebits(compressed_data_, state, 1);
        int is_smaller = 0;
        if (flag == 1) {
            run = static_cast<int>(decodebits(compressed_data_, state, 5));
            is_smaller = run % 3;
            run -= is_smaller;
            is_smaller--;
        }
        if (run > 0 && write_idx * 3 + static_cast<size_t>(run) > data.size()) {
            throw file_error("buffer overrun during decompression of XTC coordinates");
        }
        if (run > 0) {
            // read the next coordinate
            thiscoord = span<int>(intbuf_.data() + (read_idx + 1) * 3, 3);

            for (int k = 0; k < run; k += 3) {
                decodeints(compressed_data_, state, 3, smallidx, sizesmall, thiscoord);
                ++read_idx;
                thiscoord[0] += prevcoord[0] - smallnum;
                thiscoord[1] += prevcoord[1] - smallnum;
                thiscoord[2] += prevcoord[2] - smallnum;
                if (k == 0) {
                    // interchange first with second atom
                    // for better compression of water molecules
                    std::swap(thiscoord[0], prevcoord[0]);
                    std::swap(thiscoord[1], prevcoord[1]);
                    std::swap(thiscoord[2], prevcoord[2]);
                    thiscoord_fl[0] = static_cast<float>(prevcoord[0]) * inv_precision;
                    thiscoord_fl[1] = static_cast<float>(prevcoord[1]) * inv_precision;
                    thiscoord_fl[2] = static_cast<float>(prevcoord[2]) * inv_precision;
                    ++write_idx;
                    thiscoord_fl = span<float>(data.data() + write_idx * 3, 3);
                } else {
                    prevcoord[0] = thiscoord[0];
                    prevcoord[1] = thiscoord[1];
                    prevcoord[2] = thiscoord[2];
                }
                thiscoord_fl[0] = static_cast<float>(thiscoord[0]) * inv_precision;
                thiscoord_fl[1] = static_cast<float>(thiscoord[1]) * inv_precision;
                thiscoord_fl[2] = static_cast<float>(thiscoord[2]) * inv_precision;
                ++write_idx;
                thiscoord_fl = span<float>(data.data() + write_idx * 3, 3);
            }
        } else {
            thiscoord_fl[0] = static_cast<float>(thiscoord[0]) * inv_precision;
            thiscoord_fl[1] = static_cast<float>(thiscoord[1]) * inv_precision;
            thiscoord_fl[2] = static_cast<float>(thiscoord[2]) * inv_precision;
            ++write_idx;
            thiscoord_fl = span<float>(data.data() + write_idx * 3, 3);
        }
        if (is_smaller < 0) {
            --smallidx;
            smallnum = smaller;
            if (smallidx > FIRSTIDX) {
                smaller = MAGICINTS[smallidx - 1] / 2;
            } else {
                smaller = 0;
            }
        } else if (is_smaller > 0) {
            ++smallidx;
            smaller = smallnum;
            smallnum = MAGICINTS[smallidx] / 2;
        }
        sizesmall[0] = sizesmall[1] = sizesmall[2] = static_cast<unsigned int>(MAGICINTS[smallidx]);
        if (sizesmall[0] == 0 || sizesmall[1] == 0 || sizesmall[2] == 0) {
            throw file_error("invalid size found during decompression of XTC coordinates");
        }
    }

    return precision;
}

void XDRFile::write_gmx_compressed_floats(const std::vector<float>& data, float precision) {
    if (precision <= 0) {
        warning("XTC compression", "invalid precision {} <= 0, use 1000 as fallback", precision);
        precision = 1000;
    }
    write_float(precision);

    const size_t size3 = data.size();
    intbuf_.resize(size3);
    compressed_data_.resize(size3 * sizeof(int));

    assert(data.size() % 3 == 0 && "internal Error: invalid allocation size");
    const size_t natoms = data.size() / 3;

    int minint[3] = {INT_MAX, INT_MAX, INT_MAX};
    int maxint[3] = {INT_MIN, INT_MIN, INT_MIN};

    int mindiff = INT_MAX;
    int oldlint[3] = {0, 0, 0};
    for (size_t atom_idx = 0; atom_idx < natoms; ++atom_idx) {
        auto thiscoord = span<int>(intbuf_.data() + atom_idx * 3, 3);
        const auto thiscoord_fl = span<const float>(data.data() + atom_idx * 3, 3);
        int lint[3];
        for (size_t i = 0; i < 3; ++i) {
            // find nearest integer
            float lf;
            if (thiscoord_fl[i] >= 0.0f) {
                lf = thiscoord_fl[i] * precision + 0.5f;
            } else {
                lf = thiscoord_fl[i] * precision - 0.5f;
            }
            if (fabsf(lf) > static_cast<float>(INT_MAX) - 2.0) {
                // scaling would cause overflow when casting to int
                throw file_error("internal overflow compressing XTC coordinates");
            }
            lint[i] = static_cast<int>(lf);
            if (lint[i] < minint[i]) {
                minint[i] = lint[i];
            }
            if (lint[i] > maxint[i]) {
                maxint[i] = lint[i];
            }
            thiscoord[i] = lint[i];
        }
        int diff =
            abs(oldlint[0] - lint[0]) + abs(oldlint[1] - lint[1]) + abs(oldlint[2] - lint[2]);
        if (diff < mindiff && atom_idx > 0) {
            mindiff = diff;
        }
        oldlint[0] = lint[0];
        oldlint[1] = lint[1];
        oldlint[2] = lint[2];
    }
    for (size_t i = 0; i < 3; ++i) {
        write_int(minint[i]);
    }
    for (size_t i = 0; i < 3; ++i) {
        write_int(maxint[i]);
    }

    if (maxint[0] - minint[0] >= INT_MAX - 2 || maxint[1] - minint[1] >= INT_MAX - 2 ||
        maxint[2] - minint[2] >= INT_MAX - 2) {
        // turning value in unsigned by subtracting minint would cause overflow
        throw file_error("internal overflow compressing XTC coordinates");
    }

    unsigned int smallidx = FIRSTIDX;
    while (smallidx < LASTIDX && MAGICINTS[smallidx] < mindiff) {
        smallidx++;
    }
    write_int(static_cast<int32_t>(smallidx));

    unsigned int sizeint[3], bitsizeint[3];
    const unsigned int bitsize = calc_sizeint(minint, maxint, sizeint, bitsizeint);

    unsigned int tmpidx = smallidx + 8;
    unsigned int maxidx = (LASTIDX < tmpidx) ? LASTIDX : tmpidx;
    unsigned int minidx = maxidx - 8; // often this equal smallidx
    tmpidx = smallidx - 1;
    tmpidx = (FIRSTIDX > tmpidx) ? FIRSTIDX : tmpidx;

    int smaller = MAGICINTS[tmpidx] / 2;
    int smallnum = MAGICINTS[smallidx] / 2;
    unsigned int sizesmall[3];
    sizesmall[0] = sizesmall[1] = sizesmall[2] = static_cast<unsigned int>(MAGICINTS[smallidx]);
    int larger = MAGICINTS[maxidx] / 2;

    int prevrun = -1;
    unsigned int tmpcoord[8 * 3]; // max run lenght is 8*3
    int prevcoord[3] = {0, 0, 0};
    int is_smaller;
    DecodeState state = {0, 0, 0};
    for (size_t i = 0; i < natoms; ++i) {
        bool is_small = false;
        auto thiscoord = span<int>(intbuf_.data() + i * 3, 6);
        if (smallidx < maxidx && i >= 1 && abs(thiscoord[0] - prevcoord[0]) < larger &&
            abs(thiscoord[1] - prevcoord[1]) < larger &&
            abs(thiscoord[2] - prevcoord[2]) < larger) {
            is_smaller = 1;
        } else if (smallidx > minidx) {
            is_smaller = -1;
        } else {
            is_smaller = 0;
        }
        if (i + 1 < natoms) {
            if (abs(thiscoord[0] - thiscoord[3]) < smallnum &&
                abs(thiscoord[1] - thiscoord[4]) < smallnum &&
                abs(thiscoord[2] - thiscoord[5]) < smallnum) {
                // interchange first with second atom
                // for better compression of water molecules
                std::swap(thiscoord[0], thiscoord[3]);
                std::swap(thiscoord[1], thiscoord[4]);
                std::swap(thiscoord[2], thiscoord[5]);
                is_small = true;
            }
        }
        // overflow should have been checked already
        tmpcoord[0] = static_cast<unsigned int>(thiscoord[0] - minint[0]);
        tmpcoord[1] = static_cast<unsigned int>(thiscoord[1] - minint[1]);
        tmpcoord[2] = static_cast<unsigned int>(thiscoord[2] - minint[2]);
        if (bitsize == 0) {
            encodebits(compressed_data_, state, bitsizeint[0], tmpcoord[0]);
            encodebits(compressed_data_, state, bitsizeint[1], tmpcoord[1]);
            encodebits(compressed_data_, state, bitsizeint[2], tmpcoord[2]);
        } else {
            encodeints(compressed_data_, state, 3, bitsize, sizeint, tmpcoord);
        }
        prevcoord[0] = thiscoord[0];
        prevcoord[1] = thiscoord[1];
        prevcoord[2] = thiscoord[2];
        thiscoord = span<int>(intbuf_.data() + (i + 1) * 3, 3);

        if (!is_small && is_smaller == -1) {
            is_smaller = 0;
        }
        int run = 0;
        while (is_small && run < 8 * 3) {
            int tmpsum = 0;
            for (size_t j = 0; j < 3; ++j) {
                int tmp = thiscoord[j] - prevcoord[j];
                tmpsum += tmp * tmp;
            }
            if (is_smaller == -1 && tmpsum >= smaller * smaller) {
                is_smaller = 0;
            }

            // no overflow as long as is_small == true
            tmpcoord[run++] = static_cast<unsigned int>(thiscoord[0] - prevcoord[0] + smallnum);
            tmpcoord[run++] = static_cast<unsigned int>(thiscoord[1] - prevcoord[1] + smallnum);
            tmpcoord[run++] = static_cast<unsigned int>(thiscoord[2] - prevcoord[2] + smallnum);

            prevcoord[0] = thiscoord[0];
            prevcoord[1] = thiscoord[1];
            prevcoord[2] = thiscoord[2];

            ++i;
            thiscoord = span<int>(intbuf_.data() + (i + 1) * 3, 3);
            is_small = false;
            if (i < natoms && abs(thiscoord[0] - prevcoord[0]) < smallnum &&
                abs(thiscoord[1] - prevcoord[1]) < smallnum &&
                abs(thiscoord[2] - prevcoord[2]) < smallnum) {
                is_small = true;
            }
        }
        if (run != prevrun || is_smaller != 0) {
            prevrun = run;
            encodebits(compressed_data_, state, 1, 1); // flag the change in run-length
            unsigned int num = static_cast<unsigned int>(run + is_smaller + 1);
            encodebits(compressed_data_, state, 5, num);
        } else {
            encodebits(compressed_data_, state, 1, 0); // flag the fact that runlength did not change
        }
        for (int k = 0; k < run; k += 3) {
            encodeints(compressed_data_, state, 3, smallidx, sizesmall, &tmpcoord[k]);
        }
        if (is_smaller != 0) {
            if (is_smaller < 0) {
                --smallidx;
                smallnum = smaller;
                smaller = MAGICINTS[smallidx - 1] / 2;
            } else {
                ++smallidx;
                smaller = smallnum;
                smallnum = MAGICINTS[smallidx] / 2;
            }
            sizesmall[0] = sizesmall[1] = sizesmall[2] =
                static_cast<unsigned int>(MAGICINTS[smallidx]);
        }
    }
    if (state.lastbits != 0) {
        ++state.count;
    }
    assert(state.count < compressed_data_.size() && "internal Error: overflow during decompression");
    write_opaque(compressed_data_.data(), state.count);
}
