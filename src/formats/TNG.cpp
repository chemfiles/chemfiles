// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/formats/TNG.hpp"
#include "chemfiles/files/TNGFile.hpp"

#include "chemfiles/Frame.hpp"
#include "chemfiles/ErrorFmt.hpp"
using namespace chemfiles;

template<> FormatInfo chemfiles::format_information<TNGFormat>() {
    return FormatInfo("TNG").with_extension(".tng").description(
        "Trajectory New Generation binary format"
    );
}

/// A buffer for TNG allocated data. It will not allocate its own memory, but
/// will `free` the memory on destruction. It can be used in replacement of the
/// pointer type in TNG API.
template<class T> class TngBuffer {
public:
    TngBuffer(): data_(nullptr) {}
    ~TngBuffer() {
		free(data_); // NOLINT: this is an implementation of RAII
	}

    TngBuffer(const TngBuffer&) = delete;
    TngBuffer& operator=(const TngBuffer&) = delete;
    TngBuffer(TngBuffer&&) = delete;
    TngBuffer& operator=(TngBuffer&&) = delete;

    /// Get a pointer to the internal buffer
    T** ptr() {return &data_;}
    T& operator[](size_t i) {return data_[i];}
    const T& operator[](size_t i) const {return data_[i];}
private:
    T* data_;
};

#define STRING_0(x) #x
#define STRING(x) STRING_0(x)
#define CHECK(x) check_tng_error((x), (STRING(x)))

TNGFormat::TNGFormat(const std::string& path, File::Mode mode): tng_(path, mode) {}

size_t TNGFormat::nsteps() {
    int64_t n_frames = 0;
    CHECK(tng_num_frames_get(tng_, &n_frames));
    return static_cast<size_t>(n_frames);
}

void TNGFormat::read_step(size_t step, Frame& frame) {
    step_ = static_cast<int64_t>(step);
    read(frame);
}

void TNGFormat::read(Frame& frame) {
    natoms_ = 0;
    CHECK(tng_num_particles_get(tng_, &natoms_));
    assert(natoms_ > 0);
    frame.resize(static_cast<size_t>(natoms_));

    read_positions(frame);
    read_velocities(frame);
    read_cell(frame);
    read_topology(frame);

    step_++;
}

void TNGFormat::read_positions(Frame& frame) {
    TngBuffer<float> buffer;
    int64_t unused = 0;

    CHECK(tng_util_pos_read_range(tng_, step_, step_, buffer.ptr(), &unused));

    auto positions = frame.positions();
    for (size_t i=0; i<static_cast<size_t>(natoms_); i++) {
        positions[i][0] = buffer[3*i + 0];
        positions[i][1] = buffer[3*i + 1];
        positions[i][2] = buffer[3*i + 2];
    }
}

void TNGFormat::read_velocities(Frame& frame) {
    TngBuffer<float> buffer;
    int64_t unused = 0;

    auto status = tng_util_vel_read_range(
        tng_, step_, step_, buffer.ptr(), &unused
    );

    switch (status) {
    case TNG_SUCCESS:
        // Continue
        break;
    case TNG_FAILURE:
        // No velocity in this frame
        return;
    case TNG_CRITICAL:
        throw format_error(
            "fatal error in the TNG library while calling 'tng_util_vel_read_range'"
        );
    }

    frame.add_velocities();
    auto velocities = *frame.velocities();
    for (size_t i=0; i<static_cast<size_t>(natoms_); i++) {
        velocities[i][0] = buffer[3*i + 0];
        velocities[i][1] = buffer[3*i + 1];
        velocities[i][2] = buffer[3*i + 2];
    }
}

void TNGFormat::read_cell(Frame& frame) {
    TngBuffer<float> buffer;
    int64_t unused = 0;

    auto status = tng_util_box_shape_read_range(
        tng_, step_, step_, buffer.ptr(), &unused
    );

    switch (status) {
    case TNG_SUCCESS:
        // Continue
        break;
    case TNG_FAILURE:
        // No unit cell in this frame
        frame.set_cell(UnitCell());
        return;
    case TNG_CRITICAL:
        throw format_error(
            "fatal error in the TNG library while calling 'tng_util_box_shape_read_range'"
        );
    }

    auto a = Vector3D(buffer[0], buffer[1], buffer[2]);
    auto b = Vector3D(buffer[3], buffer[4], buffer[5]);
    auto c = Vector3D(buffer[6], buffer[7], buffer[8]);

    auto angle = [](const Vector3D& u, const Vector3D& v) {
        constexpr double PI = 3.141592653589793238463;
        auto cos = dot(u, v) / (u.norm() * v.norm());
        cos = std::max(-1., std::min(1., cos));
        return acos(cos) * 180.0 / PI;
    };

    double alpha = angle(b, c);
    double beta = angle(a, c);
    double gamma = angle(a, b);

    // Factor 10 because the cell lengthes are in nm in the TNG format
    frame.set_cell({a.norm() * 10, b.norm() * 10, c.norm() * 10, alpha, beta, gamma});
}

void TNGFormat::read_topology(Frame& frame) {
    auto topology = Topology();
    topology.reserve(static_cast<size_t>(natoms_));

    int64_t moltypes = 0;
    CHECK(tng_num_molecule_types_get(tng_, &moltypes));

    int64_t* molecules_counts = nullptr;
    CHECK(tng_molecule_cnt_list_get(tng_, &molecules_counts));

    // Read all molecules types
    for (int64_t moltype = 0; moltype<moltypes; moltype++) {
        tng_molecule_t molecule;
        CHECK(tng_molecule_of_index_get(tng_, moltype, &molecule));

        // Loop over all the molecules of a given type
        for (int64_t mol = 0; mol<molecules_counts[moltype]; mol++) {

            // For each type, get all the residues in the molecule
            int64_t n_residues = 0;
            CHECK(tng_molecule_num_residues_get(tng_, molecule, &n_residues));
            for (int64_t resid = 0; resid<n_residues; resid++) {
                tng_residue_t tng_residue;
                CHECK(tng_molecule_residue_of_index_get(
                    tng_, molecule, resid, &tng_residue
                ));

                char resname[32];
                CHECK(tng_residue_name_get(tng_, tng_residue, resname, 32));

                auto residue = Residue(resname);

                // And finally get all the atoms in the residue
                int64_t n_atoms = 0;
                CHECK(tng_residue_num_atoms_get(tng_, tng_residue, &n_atoms));
                for (int64_t id = 0; id<n_atoms; id++) {
                    tng_atom_t tng_atom;
                    CHECK(tng_residue_atom_of_index_get(tng_, tng_residue, id, &tng_atom));

                    char name[32];
                    CHECK(tng_atom_name_get(tng_, tng_atom, name, 32));

                    char type[32];
                    CHECK(tng_atom_type_get(tng_, tng_atom, type, 32));

                    residue.add_atom(topology.size());
                    topology.add_atom(Atom(name, type));
                }
                topology.add_residue(std::move(residue));
            }
        }
    }

    TngBuffer<int64_t> from_atoms;
    TngBuffer<int64_t> to_atoms;
    int64_t n_bonds = 0;
    CHECK(tng_molsystem_bonds_get(tng_, &n_bonds, from_atoms.ptr(), to_atoms.ptr()));

    for (size_t i=0; i<static_cast<size_t>(n_bonds); i++) {
        topology.add_bond(
            static_cast<size_t>(from_atoms[i]),
            static_cast<size_t>(to_atoms[i])
        );
    }

    frame.set_topology(topology);
}
