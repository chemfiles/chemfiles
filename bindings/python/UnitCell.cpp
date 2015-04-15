/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp-python.hpp"

void register_unit_cell() {
    /* UnitCell class *********************************************************/
    py::class_<UnitCell>("UnitCell", py::init<>())
        .def(py::init<double>())
        .def(py::init<double, double, double>())
        .def(py::init<double, double, double, double, double, double>())
        .def(py::init<UnitCell::CellType>())
        .def(py::init<UnitCell::CellType, double>())
        .def(py::init<UnitCell::CellType, double, double, double>())
        .def("matricial", &UnitCell::matricial, py::return_value_policy<Matrix3D_convertor>())
        .add_property("type",
            static_cast<UnitCell::CellType (UnitCell::*)(void) const>(&UnitCell::type),
            static_cast<void (UnitCell::*)(UnitCell::CellType)>(&UnitCell::type))
        .add_property("a",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::a),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::a))
        .add_property("b",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::b),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::b))
        .add_property("c",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::c),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::c))
        .add_property("alpha",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::alpha),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::alpha))
        .add_property("beta",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::beta),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::beta))
        .add_property("gamma",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::gamma),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::gamma))
        .add_property("periodic_x",
            static_cast<bool (UnitCell::*)(void) const>(&UnitCell::periodic_x),
            static_cast<void (UnitCell::*)(bool)>(&UnitCell::periodic_x))
        .add_property("periodic_y",
            static_cast<bool (UnitCell::*)(void) const>(&UnitCell::periodic_y),
            static_cast<void (UnitCell::*)(bool)>(&UnitCell::periodic_y))
        .add_property("periodic_z",
            static_cast<bool (UnitCell::*)(void) const>(&UnitCell::periodic_z),
            static_cast<void (UnitCell::*)(bool)>(&UnitCell::periodic_z))
        .add_property("full_periodic",
            static_cast<bool (UnitCell::*)(void) const>(&UnitCell::full_periodic),
            static_cast<void (UnitCell::*)(bool)>(&UnitCell::full_periodic))
    ;

    /* CellType enum **********************************************************/
    py::enum_<UnitCell::CellType>("CellType")
        .value("ORTHOROMBIC", UnitCell::ORTHOROMBIC)
        .value("TRICLINIC", UnitCell::TRICLINIC)
        .value("INFINITE", UnitCell::INFINITE)
    ;
}
