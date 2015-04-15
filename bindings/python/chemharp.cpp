/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp-python.hpp"

void init_module(){
    // Removing this line will result in bad stuff appening, like segfaults and
    // your grand mother being kidnaped by aliens. So don't do this!
    np::initialize();
}

BOOST_PYTHON_MODULE(chemharp){
    init_module();

    register_errors();
    register_trajectory();
    register_frame();
    register_atom();
    register_topology();
    register_unit_cell();
}
