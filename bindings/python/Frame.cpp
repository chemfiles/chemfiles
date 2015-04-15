/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp-python.hpp"

void register_frame() {
    /* Trajectory class *******************************************************/
    py::class_<Trajectory, boost::noncopyable>("Trajectory",
            py::init<string, py::optional<string, string>>())
        .def("read_next_step", &Trajectory::read_next_step)
        .def("read_at_step", &Trajectory::read_at_step)
        .def("write_step", &Trajectory::write_step)
        .def("done", &Trajectory::done)
        .def("close", &Trajectory::close)
        .def("nsteps", &Trajectory::nsteps)
        .def("topology",
            static_cast<void (Trajectory::*)(const Topology&)>(&Trajectory::topology))
        .def("topology",
            static_cast<void (Trajectory::*)(const string&)>(&Trajectory::topology))
    ;
}
