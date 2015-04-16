/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp-python.hpp"

void topology_setitem(Topology& top, size_t index, const Atom& atom) {
    top[index] = atom;
}

void register_topology() {
    /* Topology class *********************************************************/
    py::class_<Topology>("Topology")
        .def("append", &Topology::append)
        .def("remove", &Topology::remove)
        .def("add_bond", &Topology::add_bond)
        .def("remove_bond", &Topology::remove_bond)

        .def("__len__", &Topology::natoms)
        .add_property("natoms", &Topology::natoms)
        .add_property("natom_types", &Topology::natom_types)

        .def("clear", &Topology::clear)
        .def("resize", &Topology::resize)

        .def("isbond", &Topology::isbond)
        .def("isangle", &Topology::isangle)
        .def("isdihedral", &Topology::isdihedral)

        .def("guess", &Topology::guess)

        .def( "__getitem__",
                static_cast<const Atom& (Topology::*)(size_t) const>(&Topology::operator[]),
                py::return_internal_reference<>())
        .def( "__setitem__", &topology_setitem)

        /* TODO:
        vector<bond> bonds(void);
        vector<angle> angles(void);
        vector<dihedral> dihedrals(void);
        */
    ;

    py::def("dummy_topology", dummy_topology);
}
