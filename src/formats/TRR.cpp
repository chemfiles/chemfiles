// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/formats/TRR.hpp"
#include "chemfiles/files/TRRFile.hpp"

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/Frame.hpp"
using namespace chemfiles;

template <> FormatInfo chemfiles::format_information<TRRFormat>() {
    return FormatInfo("TRR").with_extension(".trr").description("TRR binary format");
}

#define STRING_0(x) #x
#define STRING(x) STRING_0(x)
#define CHECK(x) check_trr_error((x), (STRING(x)))

TRRFormat::TRRFormat(std::string path, File::Mode mode, File::Compression compression)
    : trr_(std::move(path), mode) {
    if (compression != File::DEFAULT) {
        throw format_error("TRR format does not support compression");
    }
}

size_t TRRFormat::nsteps() { return static_cast<size_t>(trr_.nframes()); }

void TRRFormat::read_step(size_t step, Frame& frame) {
    step_ = step;
    CHECK(xdr_seek(trr_, trr_.offset(step_), SEEK_SET));
    read(frame);
}

void TRRFormat::read(Frame& frame) {
    int natoms = trr_.natoms();
    int md_step = 0;
    float time = 0;
    float lambda = 0;
    matrix box;
    std::vector<float> x(static_cast<size_t>(natoms) * 3);
    std::vector<float> v(static_cast<size_t>(natoms) * 3);
    std::vector<float> f(static_cast<size_t>(natoms) * 3);
    uint8_t has_prop = 0;

    CHECK(read_trr(trr_, natoms, &md_step, &time, &lambda, box,
                   reinterpret_cast<float(*)[3]>(x.data()), reinterpret_cast<float(*)[3]>(v.data()),
                   nullptr /* ignore forces */, &has_prop));

    bool has_box = bool(has_prop & TRR_HAS_BOX);
    bool has_positions = bool(has_prop & TRR_HAS_POSITIONS);
    bool has_velocities = bool(has_prop & TRR_HAS_VELOCITIES);

    frame.set_step(static_cast<size_t>(md_step));  // actual step of MD Simulation
    frame.set("time", static_cast<double>(time));  // time in pico seconds
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

void TRRFormat::set_positions(const std::vector<float>& x, Frame& frame) {
    auto positions = frame.positions();
    for (size_t i = 0; i < static_cast<size_t>(trr_.natoms()); i++) {
        // Factor 10 because the cell lengthes are in nm in the TRR format
        positions[i][0] = static_cast<double>(x[i * 3]) * 10;
        positions[i][1] = static_cast<double>(x[i * 3 + 1]) * 10;
        positions[i][2] = static_cast<double>(x[i * 3 + 2]) * 10;
    }
}

void TRRFormat::set_velocities(const std::vector<float>& v, Frame& frame) {
    frame.add_velocities();
    auto velocities = *frame.velocities();
    for (size_t i = 0; i < static_cast<size_t>(trr_.natoms()); i++) {
        // Factor 10 because the cell lengthes are in nm in the TRR format
        velocities[i][0] = static_cast<double>(v[i * 3]) * 10;
        velocities[i][1] = static_cast<double>(v[i * 3 + 1]) * 10;
        velocities[i][2] = static_cast<double>(v[i * 3 + 2]) * 10;
    }
}

void TRRFormat::set_cell(matrix box, Frame& frame) {
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
