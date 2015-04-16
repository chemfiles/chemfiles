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
    /* Frame class ************************************************************/
    py::class_<Frame>("Frame")
        .add_property("positions",
            py::make_function(
                static_cast<const Array3D& (Frame::*)(void) const>(&Frame::positions),
                py::return_value_policy<Array3D_convertor>()),
            static_cast<void (Frame::*)(const Array3D&)>(&Frame::positions)
            )
        .add_property("velocities",
            py::make_function(
                static_cast<const Array3D& (Frame::*)(void) const>(&Frame::velocities),
                py::return_value_policy<Array3D_convertor>()),
            static_cast<void (Frame::*)(const Array3D&)>(&Frame::positions)
            )
        .add_property("has_velocities", &Frame::has_velocities)
        .def("__len__", &Frame::natoms)
        .add_property("natoms", &Frame::natoms)
        .add_property("topology",
            py::make_function(
                static_cast<const Topology& (Frame::*)(void) const>(&Frame::topology),
                py::return_value_policy<py::copy_const_reference>()),
            static_cast<void (Frame::*)(const Topology&)>(&Frame::topology))
        .add_property("cell",
            py::make_function(
                static_cast<const UnitCell& (Frame::*)(void) const>(&Frame::cell),
                py::return_value_policy<py::copy_const_reference>()),
            static_cast<void (Frame::*)(const UnitCell&)>(&Frame::cell))
        .add_property("step",
            static_cast<size_t (Frame::*)(void) const>(&Frame::step),
            static_cast<void (Frame::*)(size_t)>(&Frame::step))
    ;
}
