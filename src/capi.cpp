/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
// clang-format off
#include <cstring>

#include "chemfiles.h"
#include "chemfiles/cerrors.hpp"
#include "chemfiles.hpp"

using namespace chemfiles;

static CAPIStatus status = CAPIStatus();

const char* chfl_version(void) {
    return CHEMFILES_VERSION;
}

const char* chfl_strerror(int code) {
    return status.message(code);
}

const char* chfl_last_error(void) {
    return status.last_error.c_str();
}

int chfl_clear_errors(void) {
    CHFL_ERROR_WRAP_RETCODE(
        status = CAPIStatus();
    )
}

int chfl_loglevel(chfl_log_level_t* level) {
    assert(level != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *level = static_cast<chfl_log_level_t>(Logger::level());
    )
}

int chfl_set_loglevel(chfl_log_level_t level) {
    CHFL_ERROR_WRAP_RETCODE(
        Logger::set_level(static_cast<LogLevel>(level));
    )
}

int chfl_logfile(const char* file) {
    assert(file != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        Logger::to_file(std::string(file));
    )
}

int chfl_log_stdout(void) {
    CHFL_ERROR_WRAP_RETCODE(
        Logger::to_stdout();
    )
}

int chfl_log_stderr(void) {
    CHFL_ERROR_WRAP_RETCODE(
        Logger::to_stderr();
    )
}

int chfl_log_silent(void) {
    CHFL_ERROR_WRAP_RETCODE(
        Logger::silent();
    )
}

int chfl_log_callback(chfl_logging_cb callback) {
    CHFL_ERROR_WRAP_RETCODE(
        Logger::callback([callback](LogLevel level, const std::string& message) {
            callback(static_cast<chfl_log_level_t>(level), message.c_str());
        });
    )
}


/******************************************************************************/

CHFL_TRAJECTORY* chfl_trajectory_open(const char* filename, char mode) {
    assert(filename != nullptr);
    CHFL_TRAJECTORY* trajectory = nullptr;
    CHFL_ERROR_WRAP(
        trajectory = new Trajectory(filename, mode);
    )
    return trajectory;
error:
    delete trajectory;
    return nullptr;
}

CHFL_TRAJECTORY* chfl_trajectory_with_format(const char* filename, char mode, const char* format) {
    assert(filename != nullptr);
    assert(format != nullptr);
    CHFL_TRAJECTORY* trajectory = nullptr;
    CHFL_ERROR_WRAP(
        trajectory = new Trajectory(filename, mode, format);
    )
    return trajectory;
error:
    delete trajectory;
    return nullptr;
}

int chfl_trajectory_read_step(CHFL_TRAJECTORY *file, size_t step, CHFL_FRAME* frame) {
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *frame = file->read_step(step);
    )
}

int chfl_trajectory_read(CHFL_TRAJECTORY *file, CHFL_FRAME *frame) {
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *frame = file->read();
    )
}

int chfl_trajectory_write(CHFL_TRAJECTORY *file, const CHFL_FRAME *frame) {
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
         file->write(*frame);
    )
}

int chfl_trajectory_set_topology(CHFL_TRAJECTORY *file, const CHFL_TOPOLOGY *topology) {
    assert(file != nullptr);
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->set_topology(*topology);
    )
}

int chfl_trajectory_set_topology_file(CHFL_TRAJECTORY *file, const char* filename) {
    assert(file != nullptr);
    assert(filename != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->set_topology(std::string(filename));
    )
}

int chfl_trajectory_set_topology_with_format(CHFL_TRAJECTORY *file, const char* filename, const char* format) {
    assert(file != nullptr);
    assert(filename != nullptr);
    assert(format != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->set_topology(std::string(filename), std::string(format));
    )
}

int chfl_trajectory_set_cell(CHFL_TRAJECTORY *file, const CHFL_CELL *cell) {
    assert(file != nullptr);
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->set_cell(*cell);
    )
}

int chfl_trajectory_nsteps(CHFL_TRAJECTORY *file, size_t *nsteps) {
    assert(file != nullptr);
    assert(nsteps != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *nsteps = file->nsteps();
    )
}

int chfl_trajectory_sync(CHFL_TRAJECTORY *file) {
    assert(file != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->sync();
    )
}

int chfl_trajectory_close(CHFL_TRAJECTORY *file) {
    CHFL_ERROR_WRAP_RETCODE(
        delete file;
        file = nullptr;
    )
}

/******************************************************************************/

CHFL_FRAME* chfl_frame(size_t natoms) {
    CHFL_FRAME* frame = nullptr;
    CHFL_ERROR_WRAP(
        frame = new Frame(natoms);
    )
    return frame;
error:
    delete frame;
    return nullptr;
}

int chfl_frame_atoms_count(const CHFL_FRAME* frame, size_t *natoms) {
    assert(frame != nullptr);
    assert(natoms != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *natoms = frame->natoms();
    )
}

int chfl_frame_positions(CHFL_FRAME* frame, float (**data)[3], size_t* size) {
    assert(frame != nullptr);
    assert(data != nullptr);
    assert(size != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        auto positions = frame->positions();
        *size = positions.size();
        *data = reinterpret_cast<float(*)[3]>(positions.data());
    )
}

int chfl_frame_velocities(CHFL_FRAME* frame, float (**data)[3], size_t* size) {
    assert(frame != nullptr);
    assert(data != nullptr);
    assert(size != nullptr);
    if (!frame->velocities()) {
        status.last_error = "No velocities in this frame!";
        return CAPIStatus::MEMORY;
    }
    CHFL_ERROR_WRAP_RETCODE(
        auto velocities = frame->velocities();
        *size = velocities->size();
        *data = reinterpret_cast<float(*)[3]>(velocities->data());
    )
}

int chfl_frame_resize(CHFL_FRAME* frame, size_t natoms) {
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        frame->resize(natoms);
    )
}

int chfl_frame_add_velocities(CHFL_FRAME* frame) {
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        frame->add_velocities();
    )
}

int chfl_frame_has_velocities(const CHFL_FRAME* frame, bool* has_velocities) {
    assert(frame != nullptr);
    assert(has_velocities != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *has_velocities = bool(frame->velocities());
    )
}

int chfl_frame_set_cell(CHFL_FRAME* frame, const CHFL_CELL* cell) {
    assert(frame != nullptr);
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        frame->set_cell(*cell);
    )
}

int chfl_frame_set_topology(CHFL_FRAME* frame, const CHFL_TOPOLOGY* topology) {
    assert(frame != nullptr);
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        frame->set_topology(*topology);
    )
}

int chfl_frame_step(const CHFL_FRAME* frame, size_t* step) {
    assert(frame != nullptr);
    assert(step != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *step = frame->step();
    )
}

int chfl_frame_set_step(CHFL_FRAME* frame, size_t step) {
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        frame->set_step(step);
    )
}

int chfl_frame_guess_topology(CHFL_FRAME* frame) {
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        frame->guess_topology();
    )
}

int chfl_frame_free(CHFL_FRAME* frame) {
    CHFL_ERROR_WRAP_RETCODE(
        delete frame;
        frame = nullptr;
    )
}

/******************************************************************************/

CHFL_CELL* chfl_cell(double a, double b, double c) {
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_WRAP(
        cell = new UnitCell(a, b, c);
    )
    return cell;
error:
    delete cell;
    return nullptr;
}

CHFL_CELL* chfl_cell_triclinic(double a, double b, double c, double alpha, double beta, double gamma) {
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_WRAP(
        cell = new UnitCell(a, b, c, alpha, beta, gamma);
    )
    return cell;
error:
    delete cell;
    return nullptr;
}


CHFL_CELL* chfl_cell_from_frame(const CHFL_FRAME* frame) {
    assert(frame != nullptr);
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_WRAP(
        cell = new UnitCell();
        *cell = frame->cell();
    )
    return cell;
error:
    delete cell;
    return nullptr;
}

int chfl_cell_volume(const CHFL_CELL* cell, double* V) {
    assert(cell != nullptr);
    assert(V != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *V = cell->volume();
    )
}

int chfl_cell_lengths(const CHFL_CELL* cell, double* a, double* b, double* c) {
    assert(cell != nullptr);
    assert(a != nullptr);
    assert(b != nullptr);
    assert(c != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *a = cell->a();
        *b = cell->b();
        *c = cell->c();
    )
}

int chfl_cell_set_lengths(CHFL_CELL* cell, double a, double b, double c) {
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->set_a(a);
        cell->set_b(b);
        cell->set_c(c);
    )
}

int chfl_cell_angles(const CHFL_CELL* cell, double* alpha, double* beta, double* gamma) {
    assert(cell != nullptr);
    assert(alpha != nullptr);
    assert(beta != nullptr);
    assert(gamma != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *alpha = cell->alpha();
        *beta = cell->beta();
        *gamma = cell->gamma();
    )
}

int chfl_cell_set_angles(CHFL_CELL* cell, double alpha, double beta, double gamma) {
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->set_alpha(alpha);
        cell->set_beta(beta);
        cell->set_gamma(gamma);
    )
}

int chfl_cell_matrix(const CHFL_CELL* cell, double matrix[3][3]) {
    assert(cell != nullptr);
    assert(matrix != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->raw_matricial(matrix);
    )
}

int chfl_cell_type(const CHFL_CELL* cell, chfl_cell_type_t* type) {
    assert(cell != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *type = static_cast<chfl_cell_type_t>(cell->type());
    )
}

int chfl_cell_set_type(CHFL_CELL* cell, chfl_cell_type_t type) {
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->type(static_cast<UnitCell::CellType>(type));
    )
}

int chfl_cell_free(CHFL_CELL* cell) {
    CHFL_ERROR_WRAP_RETCODE(
        delete cell;
        cell = nullptr;
    )
}

/******************************************************************************/

CHFL_TOPOLOGY* chfl_topology_from_frame(const CHFL_FRAME* frame) {
    assert(frame != nullptr);
    CHFL_TOPOLOGY* topology = nullptr;
    CHFL_ERROR_WRAP(
        topology = new Topology();
        *topology = frame->topology();
    )
    return topology;
error:
    delete topology;
    return nullptr;
}

CHFL_TOPOLOGY* chfl_topology(void) {
    CHFL_TOPOLOGY* topology = nullptr;
    CHFL_ERROR_WRAP(
        topology = new Topology();
    )
    return topology;
error:
    delete topology;
    return nullptr;
}

int chfl_topology_atoms_count(const CHFL_TOPOLOGY* topology, size_t *natoms) {
    assert(topology != nullptr);
    assert(natoms != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *natoms = topology->natoms();
    )
}

int chfl_topology_append(CHFL_TOPOLOGY* topology, const CHFL_ATOM* atom) {
    assert(topology != nullptr);
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        topology->append(*atom);
    )
}

int chfl_topology_remove(CHFL_TOPOLOGY* topology, size_t i) {
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        topology->remove(i);
    )
}

int chfl_topology_isbond(const CHFL_TOPOLOGY* topology, size_t i, size_t j, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *result = topology->isbond(i, j);
    )
}

int chfl_topology_isangle(const CHFL_TOPOLOGY* topology, size_t i, size_t j, size_t k, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *result = topology->isangle(i, j, k);
    )
}

int chfl_topology_isdihedral(const CHFL_TOPOLOGY* topology, size_t i, size_t j, size_t k, size_t m, bool* result) {
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *result = topology->isdihedral(i, j, k, m);
    )
}

int chfl_topology_bonds_count(const CHFL_TOPOLOGY* topology, size_t* nbonds) {
    assert(topology != nullptr);
    assert(nbonds != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *nbonds = topology->bonds().size();
    )
}

int chfl_topology_angles_count(const CHFL_TOPOLOGY* topology, size_t* nangles) {
    assert(topology != nullptr);
    assert(nangles != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *nangles = topology->angles().size();
    )
}

int chfl_topology_dihedrals_count(const CHFL_TOPOLOGY* topology, size_t* ndihedrals) {
    assert(topology != nullptr);
    assert(ndihedrals != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *ndihedrals = topology->dihedrals().size();
    )
}

int chfl_topology_bonds(const CHFL_TOPOLOGY* topology, size_t (*data)[2], size_t nbonds) {
    assert(topology != nullptr);
    assert(data != nullptr);
    if (nbonds != topology->bonds().size()) {
        status.last_error = "Wrong data size in function 'chfl_topology_bonds'.";
        return CAPIStatus::MEMORY;
    }

    auto bonds = topology->bonds();
    CHFL_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<nbonds; i++) {
            data[i][0] = bonds[i][0];
            data[i][1] = bonds[i][1];
        }
    )
}

int chfl_topology_angles(const CHFL_TOPOLOGY* topology, size_t (*data)[3], size_t nangles) {
    assert(topology != nullptr);
    assert(data != nullptr);
    if (nangles != topology->angles().size()) {
        status.last_error = "Wrong data size in function 'chfl_topology_angles'.";
        return CAPIStatus::MEMORY;
    }

    auto angles = topology->angles();
    CHFL_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<nangles; i++) {
            data[i][0] = angles[i][0];
            data[i][1] = angles[i][1];
            data[i][2] = angles[i][2];
        }
    )
}

int chfl_topology_dihedrals(const CHFL_TOPOLOGY* topology, size_t (*data)[4], size_t ndihedrals) {
    assert(topology != nullptr);
    assert(data != nullptr);
    if (ndihedrals != topology->dihedrals().size()) {
        status.last_error = "Wrong data size in function 'chfl_topology_bonds'.";
        return CAPIStatus::MEMORY;
    }

    auto dihedrals = topology->dihedrals();
    CHFL_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<ndihedrals; i++) {
            data[i][0] = dihedrals[i][0];
            data[i][1] = dihedrals[i][1];
            data[i][2] = dihedrals[i][2];
            data[i][3] = dihedrals[i][3];
        }
    )
}

int chfl_topology_add_bond(CHFL_TOPOLOGY* topology, size_t i, size_t j) {
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        topology->add_bond(i, j);
    )
}

int chfl_topology_remove_bond(CHFL_TOPOLOGY* topology, size_t i, size_t j) {
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        topology->remove_bond(i, j);
    )
}

int chfl_topology_free(CHFL_TOPOLOGY* topology) {
    CHFL_ERROR_WRAP_RETCODE(
        delete topology;
        topology = nullptr;
    )
}

/******************************************************************************/

CHFL_ATOM* chfl_atom(const char* name) {
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_WRAP(
        atom = new Atom(std::string(name));
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

CHFL_ATOM* chfl_atom_from_frame(const CHFL_FRAME* frame, size_t idx) {
    assert(frame != nullptr);
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_WRAP(
        atom = new Atom("");
        *atom = frame->topology()[idx];
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

CHFL_ATOM* chfl_atom_from_topology(const CHFL_TOPOLOGY* topology, size_t idx) {
    assert(topology != nullptr);
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_WRAP(
        atom = new Atom("");
        *atom = (*topology)[idx];
    )
    return atom;
error:
    delete atom;
    return nullptr;
}

int chfl_atom_mass(const CHFL_ATOM* atom, float* mass) {
    assert(atom != nullptr);
    assert(mass != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *mass = atom->mass();
    )
}

int chfl_atom_set_mass(CHFL_ATOM* atom, float mass) {
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        atom->set_mass(mass);
    )
}

int chfl_atom_charge(const CHFL_ATOM* atom, float* charge) {
    assert(atom != nullptr);
    assert(charge != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *charge = atom->charge();
    )
}

int chfl_atom_set_charge(CHFL_ATOM* atom, float charge) {
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        atom->set_charge(charge);
    )
}

int chfl_atom_name(const CHFL_ATOM* atom, char* name, size_t buffsize) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        auto tmp = atom->name();
        strcpy(name, tmp.substr(0, buffsize).c_str());
    )
}

int chfl_atom_set_name(CHFL_ATOM* atom, const char* name) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        atom->set_name(std::string(name));
    )
}


int chfl_atom_full_name(const CHFL_ATOM* atom, char* name, size_t buffsize) {
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        std::string tmp = atom->full_name();
        strcpy(name, tmp.substr(0, buffsize).c_str());
    )
}

int chfl_atom_vdw_radius(const CHFL_ATOM* atom, double* radius) {
    assert(atom != nullptr);
    assert(radius != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *radius = atom->vdw_radius();
    )
}

int chfl_atom_covalent_radius(const CHFL_ATOM* atom, double* radius) {
    assert(atom != nullptr);
    assert(radius != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *radius = atom->covalent_radius();
    )
}

int chfl_atom_atomic_number(const CHFL_ATOM* atom, int* number) {
    assert(atom != nullptr);
    assert(number != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *number = atom->atomic_number();
    )
}

int chfl_atom_type(const CHFL_ATOM* atom, chfl_atom_type_t* type) {
    assert(atom != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *type = static_cast<chfl_atom_type_t>(atom->type());
    )
}

int chfl_atom_set_type(CHFL_ATOM* atom, chfl_atom_type_t type) {
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        atom->set_type(static_cast<Atom::AtomType>(type));
    )
}

int chfl_atom_free(CHFL_ATOM* atom) {
    CHFL_ERROR_WRAP_RETCODE(
        delete atom;
        atom = nullptr;
    )
}

/******************************************************************************/

static_assert(
    CHFL_MAX_SELECTION_SIZE == Match::MAX_MATCH_SIZE,
    "CHFL_MAX_SELECTION_SIZE should match Match::MAX_MATCH_SIZE"
);

struct CAPISelection {
    CAPISelection(Selection&& select): selection(std::move(select)), matches() {}
    Selection selection;
    std::vector<Match> matches;
};

CHFL_SELECTION* chfl_selection(const char* string) {
    CHFL_SELECTION* c_selection = nullptr;
    CHFL_ERROR_WRAP(
        c_selection = new CAPISelection(Selection(std::string(string)));
    )
    return c_selection;
error:
    delete c_selection;
    return nullptr;
}

int chfl_selection_size(const CHFL_SELECTION* c_selection, size_t* size) {
    assert(c_selection != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *size = c_selection->selection.size();
    )
}

int chfl_selection_evalutate(CHFL_SELECTION* c_selection, const CHFL_FRAME* frame, size_t* n_matches) {
    assert(c_selection != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        c_selection->matches = c_selection->selection.evaluate(*frame);
        *n_matches = c_selection->matches.size();
    )
}

int chfl_selection_matches(const CHFL_SELECTION* c_selection, chfl_match_t* matches, size_t n_matches) {
    assert(c_selection != nullptr);
    assert(n_matches == c_selection->matches.size());
    CHFL_ERROR_WRAP_RETCODE(
        auto size = c_selection->selection.size();
        for (size_t i=0; i<n_matches; i++) {
            matches[i].size = static_cast<unsigned char>(size);
            for (size_t j=0; j<size; j++) {
                matches[i].atoms[j] = c_selection->matches[i][j];
            }

            for (size_t j=size; j<CHFL_MAX_SELECTION_SIZE; j++) {
                matches[i].atoms[j] = static_cast<size_t>(-1);
            }
        }
    )
}

int chfl_selection_free(CHFL_SELECTION* c_selection) {
    CHFL_ERROR_WRAP_RETCODE(
        delete c_selection;
        c_selection = nullptr;
    )
}
