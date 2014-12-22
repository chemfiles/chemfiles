/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "harp.h"
#include "Harp.hpp"

#include "logging.hpp"

HARP_FRAME* harp_frame(const HARP_FILE*){

}

int harp_frame_size(const HARP_FRAME*){

}

int harp_frame_positions(const HARP_FRAME*, float**, const int){

}

int harp_frame_velocities(const HARP_FRAME*, float**, const int){

}

HARP_ATOM* harp_get_atom(const HARP_FRAME*, const int){

}

int harp_frame_delete(HARP_FRAME*){

}

/******************************************************************************/

float harp_get_mass(const HARP_ATOM*){

}

float harp_get_charge(const HARP_ATOM*){

}

float harp_get_radius(const HARP_ATOM*){

}

char* harp_get_name(const HARP_ATOM*, char*, const int){

}

/******************************************************************************/

HARP_FILE* harp_open(const char*){

}

int harp_read_step(const HARP_FILE*, const int, HARP_FRAME*){

}

int harp_read_next_step(const HARP_FILE*, HARP_FRAME*){

}

int harp_close(HARP_FILE*){

}
