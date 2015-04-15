/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHRP_PYTHON_HPP
#define CHRP_PYTHON_HPP

#include "Chemharp.hpp"
using namespace harp;

#include <boost/python.hpp>
namespace py = boost::python;

using std::string;

#include "convertors.hpp"

void register_errors();
void register_trajectory();
void register_frame();
void register_atom();
void register_topology();
void register_unit_cell();

#endif
