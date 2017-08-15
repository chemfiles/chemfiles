#!/usr/bin/env python
# -* coding: utf-8 -*

"""
This python script use Blue Obelisk's element.xml file to create the
corresponding C++ header with atomic information.

The element.xml file can be found at the official SVN repo:
    http://svn.code.sf.net/p/bodr/code/trunk/bodr
"""

import sys
from xml.etree import ElementTree as ET


class Atom:
    def __init__(self, symbol, name, number, mass, cov, VdW, r, g, b):
        self.symbol = symbol
        self.name = name
        self.number = number
        self.mass = mass
        self.cov = cov
        self.VdW = VdW
        self.elementColor_r = r
        self.elementColor_g = g
        self.elementColor_b = b


    def __str__(self):
        return '{{"{}", ElementData{{{}, "{}", {}, {}, {},{},{},{}}} }}'.format(
            self.symbol, self.number, self.name, self.mass, self.cov, self.VdW, self.elementColor_r, self.elementColor_g, self.elementColor_b)


def read_elements(path):
    root = ET.parse(path).getroot()
    atoms = []

    for atom in root[1:]:
        for prop in atom:
            if prop.get("dictRef") == "bo:name":
                name = prop.get("value")
            if prop.get("dictRef") == "bo:symbol":
                symbol = prop.get("value")
            if prop.get("dictRef") == "bo:atomicNumber":
                number = int(prop.text)
            if prop.get("dictRef") == "bo:mass":
                assert prop.get("units") == "units:atmass"
                mass = float(prop.text)
            if prop.get("dictRef") == "bo:radiusCovalent":
                assert prop.get("units") == "units:ang"
                cov = float(prop.text)
            if prop.get("dictRef") == "bo:radiusVDW":
                assert prop.get("units") == "units:ang"
                VdW = float(prop.text)
            if prop.get("dictRef") == "bo:elementColor":
                rgb_list = []
                reader = prop.text
                rgb_list = [float(x) for x in reader.split()]
        atoms.append(Atom(symbol, name, number, mass, cov, VdW, rgb_list[0], rgb_list[1], rgb_list[2] ))

    return atoms


HEADER = """// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

// !!!! AUTO-GENERATED FILE !!!! Do not edit. See elements.py for the code.
// The data comes from Blue Obelisk's data repository at the svn repository:
// http://svn.code.sf.net/p/bodr/code/trunk/bodr

#ifndef CHEMFILES_PERIODIC_HPP
#define CHEMFILES_PERIODIC_HPP

#include <map>
#include <string>

namespace chemfiles {
"""

STRUCT = """
//! Storing basic elemental data: mass, colvalent and Van der Waals radii, and default element colors for viewers
struct ElementData {
    //! Atomic number
    const int number;
    //! Full name
    const char* name;
    //! Mass in atomic units
    const double mass;
    //! Covalent radius in Angstrom
    const double colvalent_radius;
    //! Van der Waals radius in Angstrom
    const double vdw_radius;
    //! Default element colors for viewers
    const float elementColor_r;
    const float elementColor_g;
    const float elementColor_b;
};
"""

ARRAY = """
static const std::map<std::string, ElementData> PERIODIC_INFORMATION = {
"""


def write_elements(path, elements):
    f = open(path, "w")
    f.write(HEADER)
    f.write(STRUCT)
    f.write(ARRAY)
    for atom in atoms:
        f.write("    " + str(atom) + ",\n")
    f.write("};\n\n")  # closing the map.
    f.write("} // namespace chemfiles\n\n")  # closing the namespace.
    f.write("#endif\n")  # closing the header guard


def usage():
    print(sys.argv[0] + " path/to/elements.xml periodic.hpp")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()
        sys.exit(0)

    atoms = read_elements(sys.argv[1])
    write_elements(sys.argv[2], atoms)
