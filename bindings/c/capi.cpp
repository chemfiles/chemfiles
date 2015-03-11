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

using namespace harp;
using std::string;

const char* chrp_strerror(int errno){
    return status.message(errno);
}

const char* chrp_last_error(void){
    return status.last_error.c_str();
}

void chrp_loglevel(chrp_log_level_t level) {
    Logger::set_level(static_cast<Logger::LogLevel>(level));
}

void chrp_logfile(const char* file){
    // TODO this can throw an error
    Logger::set_log_file(string(file));
}

void chrp_log_stderr(void){
    Logger::log_to_stderr();
}

/******************************************************************************/

CHRP_TRAJECTORY* chrp_open(const char* filename, const char* mode){
    CHRP_TRAJECTORY* traj = NULL;
    CHRP_ERROR_WRAP_RETVAL(
        traj = new Trajectory(filename, mode); , traj
    )
    return traj;
}

int chrp_read_step(CHRP_TRAJECTORY *file, size_t step, CHRP_FRAME* frame){
    CHRP_ERROR_WRAP_RETCODE(
        frame = &(file->read_at_step(step));
    )
}

int chrp_read_next_step(CHRP_TRAJECTORY *file, CHRP_FRAME *frame){
    CHRP_ERROR_WRAP_RETCODE(
        frame = &(file->read_next_step());
    )
}

int chrp_write_step(CHRP_TRAJECTORY *file, CHRP_FRAME *frame){
    CHRP_ERROR_WRAP_RETCODE(
         file->write_step(*frame);
    )
}

int chrp_close(CHRP_TRAJECTORY *file){
    CHRP_ERROR_WRAP_RETCODE(
        delete file;
    )
}

/******************************************************************************/

CHRP_FRAME* chrp_frame(size_t natoms){
    CHRP_FRAME* frame = NULL;
    CHRP_ERROR_WRAP_RETVAL(
        frame = new Frame(natoms); , frame
    )
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

int chrp_frame_positions_set(CHRP_FRAME* frame, float** data, size_t size){
    // TODO
    return -1;
}

int chrp_frame_velocities(const CHRP_FRAME* frame, float (*data)[3], size_t size){
    CHRP_ERROR_WRAP_RETCODE(
        frame->raw_velocities(data, size);
    )
}

int chrp_frame_velocities_set(CHRP_FRAME* frame, float** data, size_t size){
    // TODO
    return -1;
}

int chrp_frame_free(CHRP_FRAME* frame) {
    CHRP_ERROR_WRAP_RETCODE(
        delete frame;
    )
}

/******************************************************************************/

CHRP_CELL* chrp_cell(CHRP_FRAME* frame){
    CHRP_CELL* cell = NULL;
    CHRP_ERROR_WRAP_RETVAL(
        cell = &frame->cell(); , cell
    )
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

CHRP_ATOM* chrp_atom(CHRP_FRAME* frame, size_t idx){
    CHRP_ATOM* atom = NULL;
    CHRP_ERROR_WRAP_RETVAL(
        atom = &(frame->topology()[idx]); , atom
    )
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
    std::string tmp;
    CHRP_ERROR_WRAP_RETCODE(
        tmp = atom->name();
        name = const_cast<char*>(tmp.substr(0, buffsize).c_str());
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
