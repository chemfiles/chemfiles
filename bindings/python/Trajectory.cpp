/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp-python.hpp"

void register_trajectory() {
    /* Trajectory class *******************************************************/
    py::class_<Trajectory, boost::noncopyable>("Trajectory",
            py::init<string, py::optional<string, string>>())
        .def("read", &Trajectory::read)
        .def("read_at", &Trajectory::read_at)
        .def("write", &Trajectory::write)
        .def("done", &Trajectory::done)
        .def("close", &Trajectory::close)
        .def("nsteps", &Trajectory::nsteps)
        .def("topology",
            static_cast<void (Trajectory::*)(const Topology&)>(&Trajectory::topology))
        .def("topology",
            static_cast<void (Trajectory::*)(const string&)>(&Trajectory::topology))
        .def("cell",
            static_cast<void (Trajectory::*)(const UnitCell&)>(&Trajectory::cell))
    ;
}
