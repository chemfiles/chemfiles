/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <cstring>

#include "chemfiles.h"
#include "chemfiles/cerrors.hpp"
#include "chemfiles.hpp"

using namespace chemfiles;
using std::string;

const char* chfl_version(void) {
    return CHEMFILES_VERSION;
}

const char* chfl_strerror(int code){
    return status.message(code);
}

const char* chfl_last_error(){
    return status.last_error.c_str();
}

int chfl_loglevel(chfl_log_level_t* level) {
    assert(level != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *level = static_cast<chfl_log_level_t>(Logger::level());
    )
}

int chfl_set_loglevel(chfl_log_level_t level) {
    CHFL_ERROR_WRAP_RETCODE(
        Logger::level(static_cast<Logger::LogLevel>(level));
    )
}

int chfl_logfile(const char* file){
    assert(file != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        Logger::log_to_file(string(file));
    )
}

int chfl_log_stderr(){
    CHFL_ERROR_WRAP_RETCODE(
        Logger::log_to_stderr();
    )
}

/******************************************************************************/

CHFL_TRAJECTORY* chfl_trajectory_open(const char* filename, const char* mode){
    assert(filename != nullptr);
    assert(mode != nullptr);
    CHFL_TRAJECTORY* traj = nullptr;
    CHFL_ERROR_WRAP(
        traj = new Trajectory(filename, mode);
    )
error:
    return traj;
}

CHFL_TRAJECTORY* chfl_trajectory_with_format(const char* filename, const char* mode, const char* format){
    assert(filename != nullptr);
    assert(mode != nullptr);
    assert(format != nullptr);
    CHFL_TRAJECTORY* traj = nullptr;
    CHFL_ERROR_WRAP(
        traj = new Trajectory(filename, mode, format);
    )
error:
    return traj;
}

int chfl_trajectory_read_step(CHFL_TRAJECTORY *file, size_t step, CHFL_FRAME* frame){
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *frame = file->read_step(step);
    )
}

int chfl_trajectory_read(CHFL_TRAJECTORY *file, CHFL_FRAME *frame){
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *frame = file->read();
    )
}

int chfl_trajectory_write(CHFL_TRAJECTORY *file, const CHFL_FRAME *frame){
    assert(file != nullptr);
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
         file->write(*frame);
    )
}

int chfl_trajectory_set_topology(CHFL_TRAJECTORY *file, const CHFL_TOPOLOGY *topology){
    assert(file != nullptr);
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->topology(*topology);
    )
}

int chfl_trajectory_set_topology_file(CHFL_TRAJECTORY *file, const char* filename){
    assert(file != nullptr);
    assert(filename != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->topology(string(filename));
    )
}

int chfl_trajectory_set_cell(CHFL_TRAJECTORY *file, const CHFL_CELL *cell){
    assert(file != nullptr);
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->cell(*cell);
    )
}

int chfl_trajectory_nsteps(CHFL_TRAJECTORY *file, size_t *nsteps){
    assert(file != nullptr);
    assert(nsteps != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *nsteps = file->nsteps();
    )
}

int chfl_trajectory_sync(CHFL_TRAJECTORY *file){
    assert(file != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        file->sync();
    )
}

int chfl_trajectory_close(CHFL_TRAJECTORY *file){
    assert(file != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        delete file;
    )
}

/******************************************************************************/

CHFL_FRAME* chfl_frame(size_t natoms){
    CHFL_FRAME* frame = nullptr;
    CHFL_ERROR_WRAP(
        frame = new Frame(natoms);
    )
error:
    return frame;
}

int chfl_frame_atoms_count(const CHFL_FRAME* frame, size_t *natoms){
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
        auto& positions = frame->positions();
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
        auto& velocities = frame->velocities();
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
    if (frame->velocities()) {
        return CAPIStatus::SUCCESS;
    }
    CHFL_ERROR_WRAP_RETCODE(
        auto natoms = frame->natoms();
        frame->velocities() = Array3D(natoms);
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
        frame->cell(*cell);
    )
}

int chfl_frame_set_topology(CHFL_FRAME* frame, const CHFL_TOPOLOGY* topology) {
    assert(frame != nullptr);
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        frame->topology(*topology);
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
        frame->step(step);
    )
}

int chfl_frame_guess_topology(CHFL_FRAME* frame, bool bonds){
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        frame->guess_topology(bonds);
    )
}

int chfl_frame_selection(const CHFL_FRAME* frame, const char* selection, bool matched[], size_t natoms) {
    assert(frame != nullptr);
    assert(selection != nullptr);
    assert(matched != nullptr);
    if (frame->natoms() != natoms) {
        status.last_error = "The 'select' array have a wrong size: it is "
                            + std::to_string(natoms) + " but should be " + std::to_string(frame->natoms());
        return CAPIStatus::MEMORY;
    }
    CHFL_ERROR_WRAP_RETCODE(
        auto sel = Selection(selection);
        auto matched_vec = sel.evaluate(*frame);
        for (size_t i=0; i<natoms; i++) {
            matched[i] = static_cast<bool>(matched_vec[i]);
        }
    )
}

int chfl_frame_free(CHFL_FRAME* frame) {
    assert(frame != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        delete frame;
    )
}

/******************************************************************************/

CHFL_CELL* chfl_cell(double a, double b, double c) {
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_WRAP(
        cell = new UnitCell(a, b, c);
    )
error:
    return cell;
}

CHFL_CELL* chfl_cell_triclinic(double a, double b, double c, double alpha, double beta, double gamma) {
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_WRAP(
        cell = new UnitCell(a, b, c, alpha, beta, gamma);
    )
error:
    return cell;
}


CHFL_CELL* chfl_cell_from_frame(CHFL_FRAME* frame){
    assert(frame != nullptr);
    CHFL_CELL* cell = nullptr;
    CHFL_ERROR_WRAP(
        cell = new UnitCell();
        *cell = frame->cell();
    )
error:
    return cell;
}

int chfl_cell_volume(const CHFL_CELL* cell, double* V) {
    assert(cell != nullptr);
    assert(V != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *V = cell->volume();
    )
}

int chfl_cell_lengths(const CHFL_CELL* cell, double* a, double* b, double* c){
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

int chfl_cell_set_lengths(CHFL_CELL* cell, double a, double b, double c){
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->a(a);
        cell->b(b);
        cell->c(c);
    )
}

int chfl_cell_angles(const CHFL_CELL* cell, double* alpha, double* beta, double* gamma){
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

int chfl_cell_set_angles(CHFL_CELL* cell, double alpha, double beta, double gamma){
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->alpha(alpha);
        cell->beta(beta);
        cell->gamma(gamma);
    )
}

int chfl_cell_matrix(const CHFL_CELL* cell, double matrix[3][3]){
    assert(cell != nullptr);
    assert(matrix != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->raw_matricial(matrix);
    )
}

int chfl_cell_type(const CHFL_CELL* cell, chfl_cell_type_t* type){
    assert(cell != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *type = static_cast<chfl_cell_type_t>(cell->type());
    )
}

int chfl_cell_set_type(CHFL_CELL* cell, chfl_cell_type_t type){
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->type(static_cast<UnitCell::CellType>(type));
    )
}

int chfl_cell_periodicity(const CHFL_CELL* cell, bool* x, bool* y, bool* z){
    assert(cell != nullptr);
    assert(x != nullptr);
    assert(y != nullptr);
    assert(z != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *x = cell->periodic_x();
        *y = cell->periodic_y();
        *z = cell->periodic_z();
    )
}

int chfl_cell_set_periodicity(CHFL_CELL* cell, bool x, bool y, bool z){
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        cell->periodic_x(x);
        cell->periodic_y(y);
        cell->periodic_z(z);
    )
}

int chfl_cell_free(CHFL_CELL* cell) {
    assert(cell != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        delete cell;
    )
}

/******************************************************************************/

CHFL_TOPOLOGY* chfl_topology_from_frame(CHFL_FRAME* frame){
    assert(frame != nullptr);
    CHFL_TOPOLOGY* topology = nullptr;
    CHFL_ERROR_WRAP(
        topology = new Topology();
        *topology = frame->topology();
    )
error:
    return topology;
}

CHFL_TOPOLOGY* chfl_topology() {
    CHFL_TOPOLOGY* topology = nullptr;
    CHFL_ERROR_WRAP(
        topology = new Topology();
    )
error:
    return topology;
}

int chfl_topology_atoms_count(const CHFL_TOPOLOGY* topology, size_t *natoms){
    assert(topology != nullptr);
    assert(natoms != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *natoms = topology->natoms();
    )
}

int chfl_topology_append(CHFL_TOPOLOGY* topology, const CHFL_ATOM* atom){
    assert(topology != nullptr);
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        topology->append(*atom);
    )
}

int chfl_topology_remove(CHFL_TOPOLOGY* topology, size_t i){
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        topology->remove(i);
    )
}

int chfl_topology_isbond(const CHFL_TOPOLOGY* topology, size_t i, size_t j, bool* result){
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *result = topology->isbond(i, j);
    )
}

int chfl_topology_isangle(const CHFL_TOPOLOGY* topology, size_t i, size_t j, size_t k, bool* result){
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *result = topology->isangle(i, j, k);
    )
}

int chfl_topology_isdihedral(const CHFL_TOPOLOGY* topology, size_t i, size_t j, size_t k, size_t m, bool* result){
    assert(topology != nullptr);
    assert(result != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *result = topology->isdihedral(i, j, k, m);
    )
}

int chfl_topology_bonds_count(const CHFL_TOPOLOGY* topology, size_t* nbonds){
    assert(topology != nullptr);
    assert(nbonds != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *nbonds = topology->bonds().size();
    )
}

int chfl_topology_angles_count(const CHFL_TOPOLOGY* topology, size_t* nangles){
    assert(topology != nullptr);
    assert(nangles != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *nangles = topology->angles().size();
    )
}

int chfl_topology_dihedrals_count(const CHFL_TOPOLOGY* topology, size_t* ndihedrals){
    assert(topology != nullptr);
    assert(ndihedrals != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *ndihedrals = topology->dihedrals().size();
    )
}

int chfl_topology_bonds(const CHFL_TOPOLOGY* topology, size_t (*data)[2], size_t nbonds){
    assert(topology != nullptr);
    assert(data != nullptr);
    if (nbonds != topology->bonds().size()){
        status.last_error = "Wrong data size in function 'chfl_topology_bonds'.";
        return CAPIStatus::MEMORY;
    }

    auto bonds = topology->bonds();
    CHFL_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<nbonds; i++){
            data[i][0] = bonds[i][0];
            data[i][1] = bonds[i][1];
        }
    )
}

int chfl_topology_angles(const CHFL_TOPOLOGY* topology, size_t (*data)[3], size_t nangles){
    assert(topology != nullptr);
    assert(data != nullptr);
    if (nangles != topology->angles().size()){
        status.last_error = "Wrong data size in function 'chfl_topology_angles'.";
        return CAPIStatus::MEMORY;
    }

    auto angles = topology->angles();
    CHFL_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<nangles; i++){
            data[i][0] = angles[i][0];
            data[i][1] = angles[i][1];
            data[i][2] = angles[i][2];
        }
    )
}

int chfl_topology_dihedrals(const CHFL_TOPOLOGY* topology, size_t (*data)[4], size_t ndihedrals){
    assert(topology != nullptr);
    assert(data != nullptr);
    if (ndihedrals != topology->dihedrals().size()){
        status.last_error = "Wrong data size in function 'chfl_topology_bonds'.";
        return CAPIStatus::MEMORY;
    }

    auto dihedrals = topology->dihedrals();
    CHFL_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<ndihedrals; i++){
            data[i][0] = dihedrals[i][0];
            data[i][1] = dihedrals[i][1];
            data[i][2] = dihedrals[i][2];
            data[i][3] = dihedrals[i][3];
        }
    )
}

int chfl_topology_add_bond(CHFL_TOPOLOGY* topology, size_t i, size_t j){
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        topology->add_bond(i, j);
    )
}

int chfl_topology_remove_bond(CHFL_TOPOLOGY* topology, size_t i, size_t j){
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        topology->remove_bond(i, j);
    )
}

int chfl_topology_free(CHFL_TOPOLOGY* topology){
    assert(topology != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        delete topology;
    )
}

/******************************************************************************/

CHFL_ATOM* chfl_atom(const char* name){
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_WRAP(
        atom = new Atom(string(name));
    )
error:
    return atom;
}

CHFL_ATOM* chfl_atom_from_frame(const CHFL_FRAME* frame, size_t idx){
    assert(frame != nullptr);
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_WRAP(
        atom = new Atom("");
        *atom = frame->topology()[idx];
    )
error:
    return atom;
}

CHFL_ATOM* chfl_atom_from_topology(const CHFL_TOPOLOGY* topology, size_t idx){
    assert(topology != nullptr);
    CHFL_ATOM* atom = nullptr;
    CHFL_ERROR_WRAP(
        atom = new Atom("");
        *atom = (*topology)[idx];
    )
error:
    return atom;
}

int chfl_atom_mass(const CHFL_ATOM* atom, float* mass){
    assert(atom != nullptr);
    assert(mass != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *mass = atom->mass();
    )
}

int chfl_atom_set_mass(CHFL_ATOM* atom, float mass){
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        atom->mass(mass);
    )
}

int chfl_atom_charge(const CHFL_ATOM* atom, float* charge){
    assert(atom != nullptr);
    assert(charge != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *charge = atom->charge();
    )
}

int chfl_atom_set_charge(CHFL_ATOM* atom, float charge){
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        atom->charge(charge);
    )
}

int chfl_atom_name(const CHFL_ATOM* atom, char* name, size_t buffsize){
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        auto tmp = atom->name();
        strcpy(name, tmp.substr(0, buffsize).c_str());
    )
}

int chfl_atom_set_name(CHFL_ATOM* atom, const char* name){
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        atom->name(string(name));
    )
}


int chfl_atom_full_name(const CHFL_ATOM* atom, char* name, size_t buffsize){
    assert(atom != nullptr);
    assert(name != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        string tmp = atom->full_name();
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

int chfl_atom_type(const CHFL_ATOM* atom, chfl_atom_type_t* type){
    assert(atom != nullptr);
    assert(type != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        *type = static_cast<chfl_atom_type_t>(atom->type());
    )
}

int chfl_atom_set_type(CHFL_ATOM* atom, chfl_atom_type_t type){
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        atom->type(static_cast<Atom::AtomType>(type));
    )
}

int chfl_atom_free(CHFL_ATOM* atom){
    assert(atom != nullptr);
    CHFL_ERROR_WRAP_RETCODE(
        delete atom;
    )
}
