// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cmath>
#include <cstdio>
#include <cassert>
#include <cstdint>

#include <array>
#include <string>
#include <vector>
#include <algorithm>

#include <xdrfile.h>
#include <xdrfile_trr.h>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/UnitCell.hpp"

#include "chemfiles/types.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/span.hpp"
#include "chemfiles/external/optional.hpp"

#include "chemfiles/files/XDRFile.hpp"
#include "chemfiles/formats/TRR.hpp"

using namespace chemfiles;

template <> FormatInfo chemfiles::format_information<TRRFormat>() {
    return FormatInfo("TRR").with_extension(".trr").description("TRR binary format");
}

#define STRING_0(x) #x
#define STRING(x) STRING_0(x)
#define CHECK(x) check_xdr_error((x), (STRING(x)))

static void set_positions(const std::vector<float>& x, Frame& frame);
static void get_positions(std::vector<float>& x, const Frame& frame);
static void set_velocities(const std::vector<float>& v, Frame& frame);
static void get_velocities(std::vector<float>& v, const Frame& frame);
static void set_cell(matrix box, Frame& frame);
static void get_cell(matrix box, const Frame& frame);

TRRFormat::TRRFormat(std::string path, File::Mode mode, File::Compression compression)
    : file_(XDRFile::TRR, std::move(path), mode) {
    if (compression != File::DEFAULT) {
        throw format_error("TRR format does not support compression");
    }
}

size_t TRRFormat::nsteps() { return static_cast<size_t>(file_.nframes()); }

void TRRFormat::read_step(size_t step, Frame& frame) {
    step_ = step;
    CHECK(xdr_seek(file_, file_.offset(step_), SEEK_SET));
    read(frame);
}

void TRRFormat::read(Frame& frame) {
    int natoms = file_.natoms();
    int md_step = 0;
    float time = 0;
    float lambda = 0;
    matrix box;
    std::vector<float> x(static_cast<size_t>(natoms) * 3);
    std::vector<float> v(static_cast<size_t>(natoms) * 3);
    uint8_t has_prop = 0;

    CHECK(read_trr(file_, natoms, &md_step, &time, &lambda, box,
                   reinterpret_cast<float(*)[3]>(x.data()), reinterpret_cast<float(*)[3]>(v.data()),
                   nullptr /* ignore forces */, &has_prop));

    bool has_box = bool(has_prop & TRR_HAS_BOX);
    bool has_positions = bool(has_prop & TRR_HAS_POSITIONS);
    bool has_velocities = bool(has_prop & TRR_HAS_VELOCITIES);

    frame.set_step(static_cast<size_t>(md_step));         // actual step of MD Simulation
    frame.set("time", static_cast<double>(time));         // time in pico seconds
    frame.set("trr_lambda", static_cast<double>(lambda)); // coupling parameter for free energy methods
    frame.set("has_positions", false);
    frame.resize(static_cast<size_t>(natoms));

    if (has_box) {
        set_cell(box, frame);
    }
    if (has_positions) {
        frame.set("has_positions", true);
        set_positions(x, frame);
    }
    if (has_velocities) {
        set_velocities(v, frame);
    }

    step_++;
}

void TRRFormat::write(const Frame& frame) {
    int natoms = static_cast<int>(frame.size());
    if (file_.nframes() == 0 && step_ == 0) {
        file_.set_natoms(natoms);
    } else if (natoms != file_.natoms()) {
        throw format_error(
            "TRR format does not support varying numbers of atoms: expected {}, but got {}",
            file_.natoms(), natoms);
    }

    int md_step = static_cast<int>(frame.step());
    float time = static_cast<float>(frame.get("time").value_or(0.0).as_double());
    float lambda = static_cast<float>(frame.get("trr_lambda").value_or(0.0).as_double());

    matrix box;
    std::vector<float> x;
    std::vector<float> v;
    bool has_box = frame.cell().shape() != UnitCell::INFINITE;
    if (has_box) {
        get_cell(box, frame);
    }
    if (frame.get("has_positions").value_or(true).as_bool()) {
        x.resize(static_cast<size_t>(natoms) * 3);
        get_positions(x, frame);
    }
    if (frame.velocities()) {
        v.resize(static_cast<size_t>(natoms) * 3);
        get_velocities(v, frame);
    }

    // If there is no (an infinite) box, then call `write_trr` with a `nullptr` instead
    // This makes sure, that `box_size` in `t_trnheader` is zero
    CHECK(write_trr(file_, natoms, md_step, time, lambda, has_box ? box : nullptr,
                    reinterpret_cast<float(*)[3]>(x.data()),
                    reinterpret_cast<float(*)[3]>(v.data()), nullptr /* ignore forces */));

    step_++;
}

void set_positions(const std::vector<float>& x, Frame& frame) {
    auto positions = frame.positions();
    assert(x.size() == 3 * positions.size());
    for (size_t i = 0; i < frame.size(); i++) {
        // Factor 10 because the cell lengthes are in nm in the TRR format
        positions[i][0] = static_cast<double>(x[i * 3]) * 10;
        positions[i][1] = static_cast<double>(x[i * 3 + 1]) * 10;
        positions[i][2] = static_cast<double>(x[i * 3 + 2]) * 10;
    }
}

void get_positions(std::vector<float>& x, const Frame& frame) {
    auto positions = frame.positions();
    assert(x.size() == 3 * positions.size());
    for (size_t i = 0; i < frame.size(); i++) {
        // Factor 10 because the cell lengthes are in nm in the TRR format
        x[i * 3] = static_cast<float>(positions[i][0] / 10.0);
        x[i * 3 + 1] = static_cast<float>(positions[i][1] / 10.0);
        x[i * 3 + 2] = static_cast<float>(positions[i][2] / 10.0);
    }
}

void set_velocities(const std::vector<float>& v, Frame& frame) {
    frame.add_velocities();
    auto velocities = *frame.velocities();
    assert(v.size() == 3 * velocities.size());
    for (size_t i = 0; i < frame.size(); i++) {
        // Factor 10 because the cell lengthes are in nm in the TRR format
        velocities[i][0] = static_cast<double>(v[i * 3]) * 10;
        velocities[i][1] = static_cast<double>(v[i * 3 + 1]) * 10;
        velocities[i][2] = static_cast<double>(v[i * 3 + 2]) * 10;
    }
}

void get_velocities(std::vector<float>& v, const Frame& frame) {
    auto velocities = *frame.velocities();
    assert(v.size() == 3 * velocities.size());
    for (size_t i = 0; i < frame.size(); i++) {
        // Factor 10 because the cell lengthes are in nm in the TRR format
        v[i * 3] = static_cast<float>(velocities[i][0] / 10.0);
        v[i * 3 + 1] = static_cast<float>(velocities[i][1] / 10.0);
        v[i * 3 + 2] = static_cast<float>(velocities[i][2] / 10.0);
    }
}

void set_cell(matrix box, Frame& frame) {
    auto a = Vector3D(static_cast<double>(box[0][0]), static_cast<double>(box[0][1]),
                      static_cast<double>(box[0][2]));
    auto b = Vector3D(static_cast<double>(box[1][0]), static_cast<double>(box[1][1]),
                      static_cast<double>(box[1][2]));
    auto c = Vector3D(static_cast<double>(box[2][0]), static_cast<double>(box[2][1]),
                      static_cast<double>(box[2][2]));

    auto angle = [](const Vector3D& u, const Vector3D& v) {
        constexpr double PI = 3.141592653589793238463;
        auto cos = dot(u, v) / (u.norm() * v.norm());
        cos = std::max(-1., std::min(1., cos));
        return acos(cos) * 180.0 / PI;
    };

    double alpha = angle(b, c);
    double beta = angle(a, c);
    double gamma = angle(a, b);

    // Factor 10 because the cell lengthes are in nm in the TRR format
    frame.set_cell({a.norm() * 10, b.norm() * 10, c.norm() * 10, alpha, beta, gamma});
}

void get_cell(matrix box, const Frame& frame) {
    // Factor 10 because the cell lengthes are in nm in the TRR format
    auto matrix = frame.cell().matrix() / 10.0;
    box[0][0] = static_cast<float>(matrix[0][0]);
    box[0][1] = static_cast<float>(matrix[1][0]);
    box[0][2] = static_cast<float>(matrix[2][0]);
    box[1][0] = static_cast<float>(matrix[0][1]);
    box[1][1] = static_cast<float>(matrix[1][1]);
    box[1][2] = static_cast<float>(matrix[2][1]);
    box[2][0] = static_cast<float>(matrix[0][2]);
    box[2][1] = static_cast<float>(matrix[1][2]);
    box[2][2] = static_cast<float>(matrix[2][2]);
}
