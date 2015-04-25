/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp.h"
#include "cerrors.hpp"
#include "Chemharp.hpp"

#include <cstring>

using namespace harp;
using std::string;

const char* chrp_strerror(int code){
    return status.message(code);
}

const char* chrp_last_error(){
    return status.last_error.c_str();
}

int chrp_loglevel(chrp_log_level_t level) {
    CHRP_ERROR_WRAP_RETCODE(
        Logger::level(static_cast<Logger::LogLevel>(level));
    )
}

int chrp_logfile(const char* file){
    CHRP_ERROR_WRAP_RETCODE(
        Logger::log_to_file(string(file));
    )
}

int chrp_log_stderr(){
    CHRP_ERROR_WRAP_RETCODE(
        Logger::log_to_stderr();
    )
}

/******************************************************************************/

CHRP_TRAJECTORY* chrp_open(const char* filename, const char* mode){
    CHRP_TRAJECTORY* traj = NULL;
    CHRP_ERROR_WRAP(
        traj = new Trajectory(filename, mode);
    )
error:
    return traj;
}

int chrp_trajectory_read_at(CHRP_TRAJECTORY *file, size_t step, CHRP_FRAME* frame){
    CHRP_ERROR_WRAP_RETCODE(
        *frame = file->read_at(step);
    )
}

int chrp_trajectory_read(CHRP_TRAJECTORY *file, CHRP_FRAME *frame){
    CHRP_ERROR_WRAP_RETCODE(
        *frame = file->read();
    )
}

int chrp_trajectory_write(CHRP_TRAJECTORY *file, const CHRP_FRAME *frame){
    CHRP_ERROR_WRAP_RETCODE(
         file->write(*frame);
    )
}

int chrp_trajectory_topology(CHRP_TRAJECTORY *file, CHRP_TOPOLOGY *topology){
    CHRP_ERROR_WRAP_RETCODE(
        file->topology(*topology);
    )
}

int chrp_trajectory_topology_file(CHRP_TRAJECTORY *file, const char* filename){
    CHRP_ERROR_WRAP_RETCODE(
        file->topology(string(filename));
    )
}

int chrp_trajectory_nsteps(CHRP_TRAJECTORY *file, size_t *nsteps){
    CHRP_ERROR_WRAP_RETCODE(
        *nsteps = file->nsteps();
    )
}

int chrp_trajectory_close(CHRP_TRAJECTORY *file){
    CHRP_ERROR_WRAP_RETCODE(
        delete file;
    )
}

/******************************************************************************/

CHRP_FRAME* chrp_frame(size_t natoms){
    CHRP_FRAME* frame = NULL;
    CHRP_ERROR_WRAP(
        frame = new Frame(natoms);
    )
error:
    return frame;
}

int chrp_frame_size(const CHRP_FRAME* frame, size_t *natoms){
    CHRP_ERROR_WRAP_RETCODE(
        *natoms = frame->natoms();
    )
}

int chrp_frame_positions(const CHRP_FRAME* frame, float (*data)[3] , size_t size){
    CHRP_ERROR_WRAP_RETCODE(
        frame->raw_positions(data, size);
    )
}

int chrp_frame_positions_set(CHRP_FRAME* frame, float (*data)[3], size_t size){
    if (size != frame->natoms()){
        status.last_error = "Wrong data size in function 'chrp_frame_positions_set'.";
        return CAPIStatus::MEMORY;
    }
    CHRP_ERROR_WRAP_RETCODE(
        frame->resize(size);
        auto positions = frame->positions();
        for (size_t i=0; i<frame->natoms(); i++) {
            positions[i][0] = data[i][0];
            positions[i][1] = data[i][1];
            positions[i][2] = data[i][2];
        }
    )
}

int chrp_frame_velocities(const CHRP_FRAME* frame, float (*data)[3], size_t size){
    CHRP_ERROR_WRAP_RETCODE(
        frame->raw_velocities(data, size);
    )
}

int chrp_frame_velocities_set(CHRP_FRAME* frame, float (*data)[3], size_t size){
    if (size != frame->natoms()){
        status.last_error = "Wrong data size in function 'chrp_frame_velocities_set'.";
        return CAPIStatus::MEMORY;
    }
    CHRP_ERROR_WRAP_RETCODE(
        frame->resize(size, true);
        auto velocities = frame->velocities();
        for (size_t i=0; i<frame->natoms(); i++) {
            velocities[i][0] = data[i][0];
            velocities[i][1] = data[i][1];
            velocities[i][2] = data[i][2];
        }
    )
}

int chrp_frame_has_velocities(const CHRP_FRAME* frame, bool *has_vel)  {
    CHRP_ERROR_WRAP_RETCODE(
        *has_vel = frame->has_velocities();
    )
}

int chrp_frame_cell_set(CHRP_FRAME* frame, const CHRP_CELL* cell) {
    CHRP_ERROR_WRAP_RETCODE(
        frame->cell(*cell);
    )
}

int chrp_frame_step(const CHRP_FRAME* frame, size_t* step) {
    CHRP_ERROR_WRAP_RETCODE(
        *step = frame->step();
    )
}

int chrp_frame_step_set(CHRP_FRAME* frame, size_t step) {
    CHRP_ERROR_WRAP_RETCODE(
        frame->step(step);
    )
}

int chrp_frame_free(CHRP_FRAME* frame) {
    CHRP_ERROR_WRAP_RETCODE(
        delete frame;
    )
}

/******************************************************************************/

CHRP_CELL* chrp_cell(double a, double b, double c, double alpha, double beta, double gamma) {
    CHRP_CELL* cell = NULL;
    CHRP_ERROR_WRAP(
        cell = new UnitCell(a, b, c, alpha, beta, gamma);
    )
error:
    return cell;
}


CHRP_CELL* chrp_frame_cell(CHRP_FRAME* frame){
    CHRP_CELL* cell = NULL;
    CHRP_ERROR_WRAP(
        cell = &frame->cell();
    )
error:
    return cell;
}

int chrp_cell_lengths(const CHRP_CELL* cell, double* a, double* b, double* c){
    CHRP_ERROR_WRAP_RETCODE(
        *a = cell->a();
        *b = cell->b();
        *c = cell->c();
    )
}

int chrp_cell_lengths_set(CHRP_CELL* cell, double a, double b, double c){
    CHRP_ERROR_WRAP_RETCODE(
        cell->a(a);
        cell->b(b);
        cell->c(c);
    )
}

int chrp_cell_angles(const CHRP_CELL* cell, double* alpha, double* beta, double* gamma){
    CHRP_ERROR_WRAP_RETCODE(
        *alpha = cell->alpha();
        *beta = cell->beta();
        *gamma = cell->gamma();
    )
}

int chrp_cell_angles_set(CHRP_CELL* cell, double alpha, double beta, double gamma){
    CHRP_ERROR_WRAP_RETCODE(
        cell->alpha(alpha);
        cell->beta(beta);
        cell->gamma(gamma);
    )
}

int chrp_cell_matrix(const CHRP_CELL* cell, double (*mat)[3]){
    CHRP_ERROR_WRAP_RETCODE(
        cell->raw_matricial(mat);
    )
}

int chrp_cell_type(const CHRP_CELL* cell, chrp_cell_type_t* type){
    CHRP_ERROR_WRAP_RETCODE(
        *type = static_cast<chrp_cell_type_t>(cell->type());
    )
}

int chrp_cell_type_set(CHRP_CELL* cell, chrp_cell_type_t type){
    CHRP_ERROR_WRAP_RETCODE(
        cell->type(static_cast<UnitCell::CellType>(type));
    )
}

int chrp_cell_periodicity(const CHRP_CELL* cell, bool* x, bool* y, bool* z){
    CHRP_ERROR_WRAP_RETCODE(
        *x = cell->periodic_x();
        *y = cell->periodic_y();
        *z = cell->periodic_z();
    )
}

int chrp_cell_periodicity_set(CHRP_CELL* cell, bool x, bool y, bool z){
    CHRP_ERROR_WRAP_RETCODE(
        cell->periodic_x(x);
        cell->periodic_y(y);
        cell->periodic_z(z);
    )
}

int chrp_cell_free(CHRP_CELL* cell) {
    CHRP_ERROR_WRAP_RETCODE(
        delete cell;
    )
}

/******************************************************************************/

CHRP_TOPOLOGY* chrp_topology(CHRP_FRAME* frame){
    CHRP_TOPOLOGY* topology = NULL;
    CHRP_ERROR_WRAP(
        topology = &(frame->topology());
    )
error:
    return topology;
}

CHRP_ATOM* chrp_topology_atom(CHRP_TOPOLOGY* topology, size_t idx){
    CHRP_ATOM* atom = NULL;
    CHRP_ERROR_WRAP(
        atom = &((*topology)[idx]);
    )
error:
    return atom;
}

CHRP_TOPOLOGY* chrp_empty_topology() {
    CHRP_TOPOLOGY* topology = NULL;
    CHRP_ERROR_WRAP(
        topology = new Topology();
    )
error:
    return topology;
}

int chrp_topology_size(const CHRP_TOPOLOGY* topology, size_t *natoms){
    CHRP_ERROR_WRAP_RETCODE(
        *natoms = topology->natoms();
    )
}

int chrp_topology_guess(CHRP_TOPOLOGY* topology, bool bonds){
    CHRP_ERROR_WRAP_RETCODE(
        topology->guess(bonds);
    )
}

int chrp_topology_append(CHRP_TOPOLOGY* topology, CHRP_ATOM* atom){
    CHRP_ERROR_WRAP_RETCODE(
        topology->append(*atom);
    )
}

int chrp_topology_remove(CHRP_TOPOLOGY* topology, size_t i){
    CHRP_ERROR_WRAP_RETCODE(
        topology->remove(i);
    )
}

int chrp_topology_isbond(CHRP_TOPOLOGY* topology, size_t i, size_t j, bool* result){
    CHRP_ERROR_WRAP_RETCODE(
        *result = topology->isbond(i, j);
    )
}

int chrp_topology_isangle(CHRP_TOPOLOGY* topology, size_t i, size_t j, size_t k, bool* result){
    CHRP_ERROR_WRAP_RETCODE(
        *result = topology->isangle(i, j, k);
    )
}

int chrp_topology_isdihedral(CHRP_TOPOLOGY* topology, size_t i, size_t j, size_t k, size_t m, bool* result){
    CHRP_ERROR_WRAP_RETCODE(
        *result = topology->isdihedral(i, j, k, m);
    )
}

int chrp_topology_bonds_count(CHRP_TOPOLOGY* topology, size_t* nbonds){
    CHRP_ERROR_WRAP_RETCODE(
        *nbonds = topology->bonds().size();
    )
}

int chrp_topology_angles_count(CHRP_TOPOLOGY* topology, size_t* nangles){
    CHRP_ERROR_WRAP_RETCODE(
        *nangles = topology->angles().size();
    )
}

int chrp_topology_dihedrals_count(CHRP_TOPOLOGY* topology, size_t* ndihedrals){
    CHRP_ERROR_WRAP_RETCODE(
        *ndihedrals = topology->dihedrals().size();
    )
}

int chrp_topology_bonds(CHRP_TOPOLOGY* topology, size_t (*data)[2], size_t nbonds){
    if (nbonds != topology->bonds().size()){
        status.last_error = "Wrong data size in function 'chrp_topology_bonds'.";
        return CAPIStatus::MEMORY;
    }

    auto bonds = topology->bonds();
    CHRP_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<nbonds; i++){
            data[i][0] = bonds[i][0];
            data[i][1] = bonds[i][1];
        }
    )
}

int chrp_topology_angles(CHRP_TOPOLOGY* topology, size_t (*data)[3], size_t nangles){
    if (nangles != topology->angles().size()){
        status.last_error = "Wrong data size in function 'chrp_topology_angles'.";
        return CAPIStatus::MEMORY;
    }

    auto angles = topology->angles();
    CHRP_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<nangles; i++){
            data[i][0] = angles[i][0];
            data[i][1] = angles[i][1];
            data[i][2] = angles[i][2];
        }
    )
}

int chrp_topology_dihedrals(CHRP_TOPOLOGY* topology, size_t (*data)[4], size_t ndihedrals){
    if (ndihedrals != topology->dihedrals().size()){
        status.last_error = "Wrong data size in function 'chrp_topology_bonds'.";
        return CAPIStatus::MEMORY;
    }

    auto dihedrals = topology->dihedrals();
    CHRP_ERROR_WRAP_RETCODE(
        for (size_t i = 0; i<ndihedrals; i++){
            data[i][0] = dihedrals[i][0];
            data[i][1] = dihedrals[i][1];
            data[i][2] = dihedrals[i][2];
            data[i][3] = dihedrals[i][3];
        }
    )
}

int chrp_topology_add_bond(CHRP_TOPOLOGY* topology, size_t i, size_t j){
    CHRP_ERROR_WRAP_RETCODE(
        topology->add_bond(i, j);
    )
}

int chrp_topology_remove_bond(CHRP_TOPOLOGY* topology, size_t i, size_t j){
    CHRP_ERROR_WRAP_RETCODE(
        topology->remove_bond(i, j);
    )
}

int chrp_topology_free(CHRP_TOPOLOGY* topology){
    CHRP_ERROR_WRAP_RETCODE(
        delete topology;
    )
}

/******************************************************************************/

CHRP_ATOM* chrp_atom(CHRP_FRAME* frame, size_t idx){
    CHRP_ATOM* atom = NULL;
    CHRP_ERROR_WRAP(
        atom = &(frame->topology()[idx]);
    )
error:
    return atom;
}

CHRP_ATOM* chrp_atom_from_name(const char* name){
    CHRP_ATOM* atom = NULL;
    CHRP_ERROR_WRAP(
        atom = new Atom(string(name));
    )
error:
    return atom;
}

int chrp_atom_mass(const CHRP_ATOM* atom, float* mass){
    CHRP_ERROR_WRAP_RETCODE(
        *mass = atom->mass();
    )
}

int chrp_atom_mass_set(CHRP_ATOM* atom, float mass){
    CHRP_ERROR_WRAP_RETCODE(
        atom->mass(mass);
    )
}

int chrp_atom_charge(const CHRP_ATOM* atom, float* charge){
    CHRP_ERROR_WRAP_RETCODE(
        *charge = atom->charge();
    )
}

int chrp_atom_charge_set(CHRP_ATOM* atom, float charge){
    CHRP_ERROR_WRAP_RETCODE(
        atom->charge(charge);
    )
}

int chrp_atom_name(const CHRP_ATOM* atom, char* name, size_t buffsize){
    CHRP_ERROR_WRAP_RETCODE(
        auto tmp = atom->name();
        strcpy(name, tmp.substr(0, buffsize).c_str());
    )
}

int chrp_atom_name_set(CHRP_ATOM* atom, const char* name){
    CHRP_ERROR_WRAP_RETCODE(
        atom->name(string(name));
    )
}

int chrp_atom_free(CHRP_ATOM* atom){
    CHRP_ERROR_WRAP_RETCODE(
        delete atom;
    )
}
