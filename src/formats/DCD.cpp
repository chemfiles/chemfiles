// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <utility>
#include <memory>
#include <set>

#include "chemfiles/cpp14.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"
#include "chemfiles/error_fmt.hpp"

#include "chemfiles/Frame.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/FormatMetadata.hpp"

#include "chemfiles/files/BinaryFile.hpp"
#include "chemfiles/formats/DCD.hpp"

static double cos_to_angle_degrees(double cos) {
    constexpr double pi = 3.141592653589793238463;
    return 90.0 - std::asin(cos) * 90.0 / pi / 2.0;
}

static size_t checked_cast(int32_t value) {
    if (value < 0) {
        throw chemfiles::format_error(
            "invalid value in DCD file: expected a positive integer, got {}",
            value
        );
    }
    return static_cast<size_t>(value);
}

static size_t checked_cast(int64_t value) {
    if (value < 0) {
        throw chemfiles::format_error(
            "invalid value in DCD file: expected a positive integer, got {}",
            value
        );
    }
    return static_cast<size_t>(value);
}


using namespace chemfiles;

static std::unique_ptr<BinaryFile> open_dcd_file(std::string path, File::Mode mode, bool& use_64_bit_markers) {
    // we need to check multiple variants: 32 and 64-bits fortran record
    // markers, and little or big endian files. The file should start with 84,
    // and then the magic 'CORD'.
    auto file = LittleEndianFile(path, mode);

    char data[8] = {0};
    file.read_char(data, 8);

    if (data[0] == 84 && data[1] == 0 && data[2] == 0 && data[3] == 0) {
        if (data[4] == 'C' && data[5] == 'O' && data[6] == 'R' && data[7] == 'D') {
            use_64_bit_markers = false;
            return chemfiles::make_unique<LittleEndianFile>(std::move(file));
        } else if (data[4] == 0 && data[5] == 0 && data[6] == 0 && data[7] == 0) {
            // We might be using 64-bit record markers, check for CORD
            char extra[4] = {0};
            file.read_char(extra, 4);
            if (extra[0] == 'C' && extra[1] == 'O' && extra[2] == 'R' && extra[3] == 'D') {
                use_64_bit_markers = true;
                return chemfiles::make_unique<LittleEndianFile>(std::move(file));
            }
        }
    } else if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
        if (data[3] == 84 && data[4] == 'C' && data[5] == 'O' && data[6] == 'R' && data[7] == 'D') {
            use_64_bit_markers = false;
            return chemfiles::make_unique<BigEndianFile>(path, mode);
        } else if (data[3] == 0 && data[4] == 0 && data[5] == 0 && data[6] == 0 && data[7] == 84) {
            // We might be using 64-bit record markers, check for CORD
            char extra[4] = {0};
            file.read_char(extra, 4);
            if (extra[0] == 'C' && extra[1] == 'O' && extra[2] == 'R' && extra[3] == 'D') {
                use_64_bit_markers = true;
                return chemfiles::make_unique<BigEndianFile>(path, mode);
            }
        }
    }

    // if we reach here, the file is not a DCD file
    throw format_error(
        "this is not a DCD file, first 8 bytes are 0x{:x}{:x}{:x}{:x} 0x{:x}{:x}{:x}{:x}",
        data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]
    );
}


DCDFormat::DCDFormat(std::string path, File::Mode mode, File::Compression compression):
    file_(nullptr),
    title_("")
{
    if (compression != File::DEFAULT) {
        throw format_error("compression is not supported for DCD files");
    }

    if (mode != File::READ) {
        throw format_error("write & append are not implemented yet");
    }

    try {
        file_ = open_dcd_file(path, mode, options_.use_64_bit_markers);
    } catch (const Error& e) {
        throw format_error("unable to open '{}': {}", path, e.what());
    }

    file_->seek(0);
    // read & parse the file header
    auto header_size = this->read_marker();
    assert(header_size == 84);

    // first, check the format using the last integer in the header
    auto header_start = file_->tell();

    file_->seek(header_start + 80);
    options_.charmm_version = static_cast<uint8_t>(file_->read_single_i32());
    options_.charmm_format = options_.charmm_version != 0;

    // then read the rest of the header (the first 4 bytes are CORD)
    file_->seek(header_start + 4);
    n_frames_ = checked_cast(file_->read_single_i32());
    timesteps_.start = checked_cast(file_->read_single_i32());
    timesteps_.step = checked_cast(file_->read_single_i32());

    // 20 unused bytes in the header
    file_->seek(file_->tell() + 20);
    auto n_fixed_atoms = checked_cast(file_->read_single_i32());

    if (options_.charmm_format) {
        timesteps_.dt = static_cast<double>(file_->read_single_f32());

        if (file_->read_single_i32() != 0) {
            options_.charmm_unitcell = true;
        }

        if (file_->read_single_i32() == 1) {
            options_.has_4d_data = true;
        }
    } else {
        // `timesteps_.dt` uses a 64-bite float in X-PLOR format
        timesteps_.dt = file_->read_single_f64();
    }

    file_->seek(header_start + 84);
    this->expect_marker(84);

    auto title_size = this->read_marker();
    if ((title_size - 4) % 80 != 0) {
        warning("DCD reader", "invalid title record size ({}), skipping title section", title_size);
        file_->seek(file_->tell() + static_cast<uint64_t>(title_size));
    } else {
        auto n_lines = checked_cast(file_->read_single_i32());
        if (n_lines != (title_size - 4) / 80) {
            warning(
                "DCD reader",
                "title record size ({}) does not match the number of title lines ({}), skipping title section",
                title_size, n_lines
            );
            file_->seek(file_->tell() + static_cast<uint64_t>(title_size - 4));
        }

        auto buffer = std::vector<char>(title_size - 4, '\0');
        file_->read_char(buffer);

        // each line in the title might be padded with NULL or space characters,
        // or just NULL-terminated and contain garbage. This tries to clean
        // everything up
        for (size_t line_i=0; line_i<n_lines; line_i++) {
            for (size_t char_i=line_i * 80; char_i<(line_i + 1) * 80; char_i++) {
                if (buffer[char_i] == '\0') {
                    title_ += '\n';
                    break;
                } else {
                    title_ += buffer[char_i];
                }
            }
        }
    }
    this->expect_marker(title_size);

    // the next record contains the number of atoms
    this->expect_marker(sizeof(int32_t));
    n_atoms_ = checked_cast(file_->read_single_i32());
    buffer_.resize(n_atoms_);
    this->expect_marker(sizeof(int32_t));

    n_free_atoms_ = n_atoms_;
    if (n_fixed_atoms != 0) {
        n_free_atoms_ = n_atoms_ - n_fixed_atoms;
        this->expect_marker(sizeof(int32_t) * n_free_atoms_);

        auto free_atoms = std::vector<int32_t>(n_free_atoms_);
        file_->read_i32(free_atoms);

        this->expect_marker(sizeof(int32_t) * n_free_atoms_);

        // translate from fortran 1-based indexes to 0-based indexes for
        // free atoms
        std::for_each(std::begin(free_atoms), std::end(free_atoms), [&](int32_t& index) {
            assert(index >= 1 && index <= static_cast<int32_t>(n_atoms_));
            index -= 1;
        });

        // make sure the free atoms are sorted before doing a binary search
        // with `std::lower_bound` below
        std::sort(std::begin(free_atoms), std::end(free_atoms));

        fixed_atoms_.reserve(n_atoms_);
        for (int32_t i=0; i<static_cast<int32_t>(n_atoms_); i++) {
            auto data = fixed_atom_data_t{};
            auto it = std::lower_bound(std::begin(free_atoms), std::end(free_atoms), i);
            if (it != std::end(free_atoms) && *it == i) {
                // this is a free atom
                data.fixed = false;
                data.free_index = static_cast<size_t>(it - std::begin(free_atoms));
                fixed_atoms_.push_back(data);
            } else {
                // this is a fixed atom
                data.fixed = true;
                data.fixed_coord = Vector3D();
                fixed_atoms_.push_back(data);
            }
        }
    }

    // we are done with the header, now we can compute the size of a frame and
    // use it to check if the file size and number of frames in the header match
    header_size_ = file_->tell();

    auto compute_frame_size = [&](size_t n_atoms) {
        size_t result = 0;

        size_t marker_size = options_.use_64_bit_markers ? 8 : 4;
        if (options_.charmm_format && options_.charmm_unitcell) {
            // 6 doubles and two markers for the unit cell
            result += 2 * marker_size + 6 * sizeof(double);
        }
        // three coordinates sets, each with two markers
        result += 3 * (2 * marker_size + n_atoms * sizeof(float));
        if (options_.has_4d_data) {
            // optional fourth dimension
            result += 2 * marker_size + n_atoms * sizeof(float);
        }

        return result;
    };

    first_frame_size_ = compute_frame_size(n_atoms_);
    if (fixed_atoms_.empty()) {
        frame_size_ = first_frame_size_;
    } else {
        frame_size_ = compute_frame_size(n_free_atoms_);
    }

    auto file_size = file_->file_size();
    auto n_frames_from_size = (file_size - header_size_ - first_frame_size_) / frame_size_ + 1;
    if (n_frames_from_size != n_frames_) {
        warning(
            "DCD reader",
            "the file header claims {} frames, but the file size indicate we only have {}",
            n_frames_,
            n_frames_from_size
        );
        n_frames_ = n_frames_from_size;
    }

    if (!fixed_atoms_.empty()) {
        this->read_fixed_coordinates();
    }
}

size_t DCDFormat::nsteps() {
    return n_frames_;
}

void DCDFormat::read(Frame& frame) {
    this->read_step(step_, frame);
    step_++;
}

void DCDFormat::read_step(size_t step, Frame& frame) {
    step_ = step;

    if (step_ == 0) {
        file_->seek(header_size_);
    } else {
        file_->seek(header_size_ + first_frame_size_ + (step_ - 1) * frame_size_);
    }

    frame.set_cell(this->read_cell());
    read_positions(frame);

    // set frame properties
    // TODO: document properties
    frame.set("time", timesteps_.dt * (timesteps_.step * step_ + timesteps_.start));
    frame.set("title", title_);
}

void DCDFormat::write(const Frame& frame) {
    throw format_error("not yet implemented");
}


size_t DCDFormat::read_marker() {
    if (options_.use_64_bit_markers) {
        return checked_cast(file_->read_single_i64());
    } else {
        return checked_cast(file_->read_single_i32());
    }
}

void DCDFormat::expect_marker(size_t size) {
    auto marker = this->read_marker();
    if (marker != size) {
        throw format_error("invalid fortran record marker, expected {} got {}", size, marker);
    }
}

UnitCell DCDFormat::read_cell() {
    if (!options_.charmm_format || !options_.charmm_unitcell) {
        return {};
    }

    this->expect_marker(48);
    double buffer[6] = {0};
    file_->read_f64(buffer, 6);
    this->expect_marker(48);

    if (options_.charmm_format && options_.charmm_version > 25) {
        // recent charmm version store cell vectors directly. It seems to be
        // doing this since CHARMM 22, but the readdcd code pretends to be
        // CHARMM 24 most of the time, so we need to check for a higher version.
        auto matrix = Matrix3D(
            buffer[0], buffer[1], buffer[3],
            buffer[1], buffer[2], buffer[4],
            buffer[3], buffer[4], buffer[5]
        );
        return UnitCell(matrix);
    }

    auto lengths = Vector3D(buffer[0], buffer[2], buffer[5]);
    auto alpha = buffer[4];
    auto beta = buffer[3];
    auto gamma = buffer[1];

    if (std::fabs(alpha) <= 1.0 && std::fabs(beta) <= 1.0 && std::fabs(gamma) <= 1.0) {
        // cell angles are either stored directly or as the cos of the angle
        alpha = cos_to_angle_degrees(alpha);
        beta = cos_to_angle_degrees(beta);
        gamma = cos_to_angle_degrees(gamma);
    }

    auto is_degree_angle = [](double angle) {
        return angle >= 0.0 && angle <= 180.0;
    };

    if (
        lengths[0] >= 0.0 && lengths[1] >= 0.0 && lengths[2] >= 0.0 &&
        is_degree_angle(alpha) && is_degree_angle(beta) && is_degree_angle(gamma)
    ) {
        auto angles = Vector3D(alpha, beta, gamma);
        return UnitCell(lengths, angles);
    } else {
        warning(
            "DCD reader",
            "unable to guess unit cell convention. The cell is stored as [{} {} {} {} {} {}]",
            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]
        );
        return UnitCell();
    }
}

void DCDFormat::read_positions(Frame& frame) {
    frame.resize(n_atoms_);
    auto positions = frame.positions();

    auto n_atoms_to_read = n_atoms_;
    if (!fixed_atoms_.empty()) {
        if (step_ != 0) {
            n_atoms_to_read = n_free_atoms_;
            for (size_t i=0; i<frame.size(); i++) {
                if (fixed_atoms_[i].fixed) {
                    positions[i] = fixed_atoms_[i].fixed_coord;
                }
            }
        }
    }

    buffer_.resize(n_atoms_to_read);

    // read the X coordinate
    this->expect_marker(sizeof(float) * n_atoms_to_read);
    file_->read_f32(buffer_);
    this->expect_marker(sizeof(float) * n_atoms_to_read);

    if (n_atoms_to_read == n_atoms_) {
        for (size_t i=0; i<n_atoms_; i++) {
            positions[i][0] = static_cast<double>(buffer_[i]);
        }
    } else {
        for (size_t i=0; i<frame.size(); i++) {
            if (!fixed_atoms_[i].fixed) {
                positions[i][0] = static_cast<double>(buffer_[fixed_atoms_[i].free_index]);
            }
        }
    }

    // read the Y coordinate
    this->expect_marker(sizeof(float) * n_atoms_to_read);
    file_->read_f32(buffer_);
    this->expect_marker(sizeof(float) * n_atoms_to_read);

    if (n_atoms_to_read == n_atoms_) {
        for (size_t i=0; i<n_atoms_; i++) {
            positions[i][1] = static_cast<double>(buffer_[i]);
        }
    } else {
        for (size_t i=0; i<frame.size(); i++) {
            if (!fixed_atoms_[i].fixed) {
                positions[i][1] = static_cast<double>(buffer_[fixed_atoms_[i].free_index]);
            }
        }
    }

    // read the Z coordinate
    this->expect_marker(sizeof(float) * n_atoms_to_read);
    file_->read_f32(buffer_);
    this->expect_marker(sizeof(float) * n_atoms_to_read);

    if (n_atoms_to_read == n_atoms_) {
        for (size_t i=0; i<n_atoms_; i++) {
            positions[i][2] = static_cast<double>(buffer_[i]);
        }
    } else {
        for (size_t i=0; i<frame.size(); i++) {
            if (!fixed_atoms_[i].fixed) {
                positions[i][2] = static_cast<double>(buffer_[fixed_atoms_[i].free_index]);
            }
        }
    }

    if (options_.has_4d_data) {
        // skip the fourth dimension
        this->expect_marker(sizeof(float) * n_atoms_to_read);
        file_->seek(file_->tell() + sizeof(float) * n_atoms_to_read);
        this->expect_marker(sizeof(float) * n_atoms_to_read);
    }
}

void DCDFormat::read_fixed_coordinates() {
    auto frame = Frame();
    this->read_step(0, frame);
    assert(fixed_atoms_.size() == frame.size());

    auto positions = frame.positions();
    for (size_t i=0; i<frame.size(); i++) {
        if (fixed_atoms_[i].fixed) {
            fixed_atoms_[i].fixed_coord = positions[i];
        }
    }
}

/******************************************************************************/

template <> const FormatMetadata& chemfiles::format_metadata<DCDFormat>() {
    static FormatMetadata metadata;
    metadata.name = "DCD";
    metadata.extension = ".dcd";
    metadata.description = "DCD binary format";
    metadata.reference = "http://www.ks.uiuc.edu/Research/vmd/plugins/molfile/dcdplugin.html";

    metadata.read = true;
    metadata.write = false;
    metadata.memory = false;

    metadata.positions = true;
    metadata.velocities = false;
    metadata.unit_cell = true;
    metadata.atoms = false;
    metadata.bonds = false;
    metadata.residues = false;
    return metadata;
}
