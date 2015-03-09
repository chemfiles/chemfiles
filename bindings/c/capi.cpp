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

static const CAPIStatus status = CAPIStatus();

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

}

int chrp_read_step(const CHRP_TRAJECTORY *file, const size_t step, CHRP_FRAME* frame){

}

int chrp_read_next_step(const CHRP_TRAJECTORY *file, CHRP_FRAME *frame){

}

int chrp_write_step(const CHRP_TRAJECTORY *file, CHRP_FRAME *frame){

}

int chrp_close(CHRP_TRAJECTORY *file){

}

/******************************************************************************/

CHRP_FRAME* chrp_frame(const size_t natoms){

}

int chrp_frame_size(const CHRP_FRAME* frame, size_t *natoms){

}

int chrp_frame_positions(const CHRP_FRAME* frame, float** data, const size_t size){

}

int chrp_frame_positions_set(CHRP_FRAME* frame, float** data, const size_t size){

}

int chrp_frame_velocities(const CHRP_FRAME* frame, float** data, const size_t size){

}

int chrp_frame_velocities_set(CHRP_FRAME* frame, float** data, const size_t size){

}

int chrp_frame_free(CHRP_FRAME* frame);

/******************************************************************************/

CHRP_CELL* chrp_cell(const CHRP_FRAME* frame){

}

int chrp_cell_lengths(const CHRP_CELL* cell, double* a, double* b, double* c){

}

int chrp_cell_lengths_set(CHRP_CELL* cell, double* a, double* b, double* c){

}

int chrp_cell_angles(const CHRP_CELL* cell, double* alpha, double* beta, double* gamma){

}

int chrp_cell_angles_set(CHRP_CELL* cell, double* alpha, double* beta, double* gamma){

}

int chrp_cell_matrix(const CHRP_CELL* cell, double* mat[]){

}

int chrp_cell_type(const CHRP_CELL* cell, chrp_cell_type_t* type){

}

int chrp_cell_type_set(CHRP_CELL* cell, chrp_cell_type_t type){

}

int chrp_cell_periodicity(const CHRP_CELL* cell, bool* x, bool* y, bool* z){

}

int chrp_cell_periodicity_set(CHRP_CELL* cell, bool* x, bool* y, bool* z){

}

int chrp_cell_free(CHRP_CELL* cell);

/******************************************************************************/

CHRP_ATOM* chrp_atom(const CHRP_TOPOLOGY* topology, const size_t idx){

}

int chrp_atom_mass(const CHRP_ATOM* atom, float* mass){

}

int chrp_atom_mass_set(CHRP_ATOM* atom, float mass){

}

int chrp_atom_charge(const CHRP_ATOM* atom, float* charge){

}

int chrp_atom_charge_set(CHRP_ATOM* atom, float charge){

}

int chrp_atom_name(const CHRP_ATOM* atom, char* name, const size_t buffsize){

}

int chrp_atom_name_set(CHRP_ATOM* atom, const char* name){

}

int chrp_atom_free(CHRP_ATOM* atoms){

}
