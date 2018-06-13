// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cstring>
#include <cassert>
#include <limits>

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/files/XzFile.hpp"
using namespace chemfiles;

static void check(lzma_ret code) {
    switch (code) {
    case LZMA_OK:
    case LZMA_STREAM_END:
        return;
    case LZMA_MEM_ERROR:
        throw file_error("xzstreambuf: Memory allocation failed (code: {})", code);
    case LZMA_FORMAT_ERROR:
        throw file_error("xzstreambuf: Input not in .xz format (code: {})", code);
    case LZMA_OPTIONS_ERROR:
        throw file_error(
            "xzstreambuf: Unsupported compression options (code: {})", code);
    case LZMA_DATA_ERROR:
        throw file_error("xzstreambuf: Compressed file is corrupted (code: {})", code);
    case LZMA_BUF_ERROR:
        throw file_error(
            "xzstreambuf: Compressed file is truncated or corrupted (code: {})", code);
    case LZMA_UNSUPPORTED_CHECK:
        throw file_error("xzstreambuf: Specified integrity check is not supported (code: {})", code);
    default:
        throw file_error("xzstreambuf: Unknown error (code: {})", code);
    }
}

xzstreambuf::xzstreambuf(size_t buffer_size):
    file_(nullptr),
    stream_(LZMA_STREAM_INIT),
    filters_({{{LZMA_VLI_UNKNOWN, NULL}}}),
    action_(LZMA_RUN),
    index_(nullptr),
    in_buffer_(buffer_size),
    out_buffer_(buffer_size),
    decoded_position_(0),
    discard_amount_(0),
    at_block_boundary_(true),
    reading_(true)
{
    stream_.next_in = nullptr;
    stream_.avail_in = 0;
    stream_.next_out = nullptr;
    stream_.avail_out = 0;

    auto end = &out_buffer_.back() + 1;
    setg(end, end, end);
    setp(&in_buffer_.front(), &in_buffer_.back());

    block_.filters = filters_.data();
}

xzstreambuf::~xzstreambuf() {
    if (!reading_) {
        action_ = LZMA_FINISH;
        sync();
    }

    if (index_) {
        lzma_index_end(index_, nullptr);
    }

    for (size_t i = 0; i < LZMA_FILTERS_MAX; ++i) {
        free(filters_[i].options);
    }

    lzma_end(&stream_);
    if (file_) {
        fclose(file_);
    }
}

void xzstreambuf::open(const std::string& path, const std::string& mode) {
    if (is_open()) {
        throw file_error("can not open an xz file twice with the same xzstreambuf");
    }
    file_ = fopen(path.c_str(), mode.c_str());

    if (mode == "wb") {
        reading_ = false;
        check(lzma_easy_encoder(&stream_, 6, LZMA_CHECK_CRC64));
    } else if (mode == "rb") {
        reading_ = true;
        std::array<uint8_t, LZMA_STREAM_HEADER_SIZE> buffer = {{0}};
        if (!std::fread(buffer.data(), buffer.size(), sizeof(uint8_t), file_)) {
            throw file_error("error while reading lzma header: {}", strerror(errno));
        }
        lzma_stream_flags flags;
        check(lzma_stream_header_decode(&flags, buffer.data()));
        check_ = flags.check;
    } else {
        throw file_error("xzstreambuf: unrecognized open mode: '{}'", mode);
    }

    stream_.next_out = reinterpret_cast<uint8_t*>(out_buffer_.data());
    stream_.avail_out = out_buffer_.size();
}

int xzstreambuf::overflow(int ch) {
    if (ch != traits_type::eof()) {
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);
        if (sync() == 0) {
            return ch;
        }
    }

    return traits_type::eof();
}

int xzstreambuf::sync() {
    if (reading_) {
        return std::streambuf::sync();
    }

    auto bytes = pptr() - pbase();
    stream_.next_in = reinterpret_cast<uint8_t*>(pbase());
    stream_.avail_in = static_cast<size_t>(bytes);

    // Two cases:
    // 1. We are still compressing the file, in which case we should pump
    //    the loop until all of the available input bytes are consumed; or
    //
    // 2. We are done receiving input (action_ == LZMA_FINISH), in which
    //    case we should pump the loop until we get the LZMA_STREAM_END
    //    return code indicating that all input has been processed (note
    //    that processed != read, hence this second case).
    lzma_ret status;
    do {
        status = lzma_code(&stream_, action_);

        if (stream_.avail_out == 0 || status == LZMA_STREAM_END) {
            auto size = out_buffer_.size() - stream_.avail_out;
            if (std::fwrite(out_buffer_.data(), sizeof(uint8_t), size, file_) != size) {
                return -1;
            }

            stream_.next_out = reinterpret_cast<uint8_t*>(out_buffer_.data());
            stream_.avail_out = out_buffer_.size();
        }

        check(status);

    } while (stream_.avail_in > 0 || (action_ == LZMA_FINISH && status != LZMA_STREAM_END));

    if (bytes > 0) {
        pbump(-static_cast<int>(bytes));
    }

    return 0;
}

int xzstreambuf::underflow() {
    if (!file_ || !reading_) {
        return traits_type::eof();
    }
    if (gptr() < egptr()) {
        // buffer not exhausted
        return traits_type::to_int_type(*gptr());
    }

    lzma_ret status = LZMA_OK;
    while (gptr() >= egptr() && status == LZMA_OK) {
        stream_.next_out = reinterpret_cast<uint8_t*>(out_buffer_.data());
        stream_.avail_out = out_buffer_.size();

        if (at_block_boundary_) {
            std::vector<std::uint8_t> header_buf(LZMA_BLOCK_HEADER_SIZE_MAX);
            if (stream_.avail_in == 0 && !std::feof(file_) && !std::ferror(file_)) {
                replenish_compressed_buffer();
            }
            assert(stream_.avail_in > 0);
            std::memcpy(header_buf.data(), stream_.next_in, 1);
            ++(stream_.next_in);
            --(stream_.avail_in);

            if (header_buf[0] == 0x00) {
                // Index indicator found
                status = LZMA_STREAM_END;
            } else {
                // free previously allocated filters in the block. We can use
                // `free` directly because no allocator is passed to any lzma
                // calls
                for (size_t i = 0; i < LZMA_FILTERS_MAX; ++i) {
                    free(filters_[i].options);
                }

                block_.version = 0;
                block_.check = check_;
                block_.header_size = lzma_block_header_size_decode(header_buf[0]);

                size_t bytes_already_copied = 0;
                if (stream_.avail_in < (block_.header_size - 1)) {
                    bytes_already_copied = stream_.avail_in;
                    std::memcpy(&header_buf[1], stream_.next_in, bytes_already_copied);
                    stream_.avail_in -= bytes_already_copied;
                    stream_.next_in += bytes_already_copied;
                    assert(stream_.avail_in == 0);
                    replenish_compressed_buffer();
                }

                assert(stream_.avail_in >= (block_.header_size - 1) - bytes_already_copied);
                size_t bytes_left_to_copy = (block_.header_size - 1) - bytes_already_copied;
                std::memcpy(&header_buf[1 + bytes_already_copied], stream_.next_in, bytes_left_to_copy);
                stream_.avail_in -= bytes_left_to_copy;
                stream_.next_in += bytes_left_to_copy;

                status = lzma_block_header_decode(&block_, nullptr, header_buf.data());
                check(status);
                status = lzma_block_decoder(&stream_, &block_);
                check(status);
            }
            at_block_boundary_ = false;
        }

        if (status == LZMA_OK) {
            if (stream_.avail_in == 0 && !std::feof(file_) && !std::ferror(file_)) {
                replenish_compressed_buffer();
            }

            assert(stream_.avail_in > 0);
            status = lzma_code(&stream_, LZMA_RUN);
            if (status == LZMA_STREAM_END) {
                // End of block.
                at_block_boundary_ = true;
                status = LZMA_OK;
            }
        }

        auto start = out_buffer_.data();
        setg(start, start, start + (out_buffer_.size() - stream_.avail_out));
        decoded_position_ += static_cast<uint64_t>(egptr() - gptr());

        if (discard_amount_ > 0) {
            uint64_t advance_amount = discard_amount_;
            if (static_cast<uint64_t>(egptr() - gptr()) < advance_amount) {
                advance_amount = static_cast<uint64_t>(egptr() - gptr());
            }
            setg(start, gptr() + advance_amount, egptr());
            discard_amount_ -= advance_amount;
        }
    }

    if (status == LZMA_STREAM_END && gptr() >= egptr()) {
        return traits_type::eof();
    } else {
        // throw an error if neeeded
        check(status);
    }

    return traits_type::to_int_type(*gptr());
}

xzstreambuf::pos_type xzstreambuf::seekoff(std::streambuf::off_type offset,
                                           std::ios_base::seekdir way,
                                           std::ios_base::openmode which) {
    auto position = pos_type(off_type(decoded_position_) - (egptr() - gptr()));
    auto current_position = position;

    // Fast return path for tellg
    if (offset == 0 && way == std::ios::cur) {
        return position;
    }

    if (way == std::ios::cur) {
        position = position + offset;
    } else if (way == std::ios::end) {
        if (!index_) {
            if (!init_index()) {
                return pos_type(off_type(-1));
            }
        }

        auto uncompressed_size = lzma_index_uncompressed_size(index_);
        position = pos_type(off_type(uncompressed_size)) + offset;
    } else {
        position = offset;
    }

    // Do nothing if we are already at the right position
    if (position == current_position) {
        return position;
    }

    return seekpos(position, which);
}

xzstreambuf::pos_type xzstreambuf::seekpos(std::streambuf::pos_type position,
                                           std::ios_base::openmode /*which*/) {
    if (file_ == 0 || sync()) {
        return pos_type(off_type(-1));
    }

    if (!index_) {
        if (!init_index()) {
            return pos_type(off_type(-1));
        }
    }

    lzma_index_iter iter;
    lzma_index_iter_init(&iter, index_);

    // Returns true on failure.
    if (lzma_index_iter_locate(&iter, static_cast<uint64_t>(off_type(position)))) {
        return pos_type(off_type(-1));
    }

    long seek_amount = 0;
    if (iter.block.compressed_file_offset > std::numeric_limits<long>::max()) {
        seek_amount = std::numeric_limits<long>::max();
    } else {
        seek_amount = static_cast<long>(iter.block.compressed_file_offset);
    }
    if (fseek(file_, seek_amount, SEEK_SET)) {
        return pos_type(off_type(-1));
    }

    decoded_position_ = iter.block.uncompressed_file_offset;
    discard_amount_ = static_cast<uint64_t>(off_type(position)) - decoded_position_;

    at_block_boundary_ = true;
    stream_.next_in = nullptr;
    stream_.avail_in = 0;
    auto end = &out_buffer_.back() + 1;
    setg(end, end, end);

    return position;
}

void xzstreambuf::replenish_compressed_buffer() {
    stream_.next_in = reinterpret_cast<uint8_t*>(in_buffer_.data());
    stream_.avail_in = std::fread(in_buffer_.data(), 1, in_buffer_.size(), file_);
}

bool xzstreambuf::init_index() {
    if (!file_) {
        return false;
    }

    std::array<uint8_t, LZMA_STREAM_HEADER_SIZE> buffer = {{0}};
    if (fseek(file_, -12, SEEK_END) || !std::fread(buffer.data(), 12, 1, file_)) {
        return false;
    }

    lzma_stream_flags footer_flags;
    if (lzma_stream_footer_decode(&footer_flags, buffer.data()) != LZMA_OK) {
        return false;
    }

    std::vector<uint8_t> index_buf(static_cast<size_t>(footer_flags.backward_size));
    auto size = -static_cast<long>(footer_flags.backward_size + 12);
    if (fseek(file_, size, SEEK_END) || !std::fread(index_buf.data(), index_buf.size(), 1, file_)) {
        return false;
    }

    uint64_t memlimit = UINT64_MAX;
    size_t in_pos = 0;
    auto res = lzma_index_buffer_decode(&index_, &memlimit, nullptr, index_buf.data(), &in_pos, index_buf.size());
    if (res != LZMA_OK) {
        return false;
    }

    return true;
}

bool xzstreambuf::is_open() const {
    return file_ != nullptr && !::ferror(file_);
}

XzFile::XzFile(const std::string& filename, File::Mode mode): TextFile(filename, mode, &buffer_), buffer_() {
    if (mode == File::READ) {
        buffer_.open(filename, "rb");
    } else if (mode == File::WRITE) {
        buffer_.open(filename, "wb");
    } else if (mode == File::APPEND) {
        throw file_error("appending (open mode 'a') is not supported with xz files");
    }

    if (!buffer_.is_open()) {
        throw file_error("could not open the file at {}", filename);
    }
}
