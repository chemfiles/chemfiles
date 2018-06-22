#!/usr/bin/env python
# -* coding: utf-8 -*

"""
This python script use Blue Obelisk's elements.xml file to create the
corresponding C++ header with atomic information.

The elements.xml file can be found at the official SVN repo:
    http://svn.code.sf.net/p/bodr/code/trunk/bodr
"""

import sys
from xml.etree import ElementTree as ET


class Atom:
    def __init__(self, symbol, name, number, mass, cov, VdW):
        self.symbol = symbol
        self.name = name
        self.number = number
        self.mass = mass
        self.cov = cov
        self.VdW = VdW

    def __str__(self):
        return '{{"{}", {{{}, std::string("{}"), {}, nullopt, {}, {}}}}}'.format(
            self.symbol, self.number, self.name, self.mass, self.cov, self.VdW
        )


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
        atoms.append(Atom(symbol, name, number, mass, cov, VdW))

    return atoms


HEADER = """// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

// !!!! AUTO-GENERATED FILE !!!! Do not edit. See elements.py for the code.
// The data comes from Blue Obelisk's data repository at the svn repository:
// http://svn.code.sf.net/p/bodr/code/trunk/bodr

#include "chemfiles/periodic_table.hpp"
using namespace chemfiles;
"""

ARRAY = """
const atomic_data_map chemfiles::PERIODIC_TABLE = {
"""


def write_elements(path, elements):
    with open(path, "w") as fd:
        fd.write(HEADER)
        fd.write(ARRAY)
        for atom in atoms:
            fd.write("    " + str(atom) + ",\n")
        fd.write("};\n")


def usage():
    print(sys.argv[0] + " path/to/elements.xml periodic_table.cpp")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()
        sys.exit(0)

    atoms = read_elements(sys.argv[1])
    write_elements(sys.argv[2], atoms)
