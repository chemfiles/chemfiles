#!/usr/bin/env python
# -* coding: utf-8 -*

"""
This script reads the standard connectivty table provided by
the PDB and produces a C++ header file that maps residue names
to their connectivity by atom name.

WARNING: The entire PDB dictionary is quite large, so it is recomended to
only use part of it (see usage).

The online version of this table is available here:

http://ftp.wwpdb.org/pub/pdb/data/monomers/het_dictionary.txt

"""

import sys
import re

class Residue:
    def __init__(self, three_letter_code):
        self.three_letter_code = three_letter_code.strip()
        self.atom_names = {}

    def __str__(self):
        return '{"' + self.three_letter_code + '", {\n' + self.connectivity() + '    }}'

    def add_atom_name(self, array):
        self.atom_names[array[0]] = array[2:-1]

    def connectivity(self):
        result = ""
        for atom in self.atom_names:
            for connected in self.atom_names[atom]:
                result += '        {"' + atom + '","' + connected + '"},\n'
        return result
        

def read_residues(path, accepted_residues = []):

    residues = []
    read_residue = False

    with open(path) as f:
        for line in f:
            line = line.replace(r'"', r'\"')
            if line[:7] == "RESIDUE":
                read_residue = False
                if len(accepted_residues) == 0 or line[10:13] in accepted_residues:
                    residues.append(Residue(line[10:13]))
                    read_residue = True
            if line[:6] == "CONECT" and read_residue:
                residues[-1].add_atom_name(re.split(r'\s+', line)[1:])

    return residues

HEADER = """// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
// !!!! AUTO-GENERATED FILE !!!! Do not edit. See pdb_connectivity.py for the code.
// The data comes from the PDB's connectivity table:
// http://ftp.wwpdb.org/pub/pdb/data/monomers/het_dictionary.txt
#ifndef CHEMFILES_PDBCONNECTIVITY_HPP
#define CHEMFILES_PDBCONNECTIVITY_HPP
#include <map>
#include <string>
#include <cstdint>
#include <vector>
namespace chemfiles {
"""

TYPEDEF = """
//! Storing a list of atom names that can be used to assign connectivity
typedef std::multimap<std::string, std::string> ResidueConnectMap;
"""

MAP = """
static const std::map<std::string, ResidueConnectMap> PDB_CONNECTIVITY_INFORMATION = {
"""

def write_elements(path, residues):
    f = open(path, "w")
    f.write(HEADER)

    for res in residues:
        f.write("// Added: " + res.three_letter_code + "\n")

    f.write(TYPEDEF)
    f.write(MAP)
    for residue in residues:
        f.write("    " + str(residue) + ",\n")
    f.write("};\n\n")  # closing the map.
    f.write("} // namespace chemfiles\n\n")  # closing the namespace.
    f.write("#endif\n")  # closing the header guard


def usage():
    print(sys.argv[0] + " path/to/het_dictionary.txt pdb_connectivity.hpp [Residue names]")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()
        sys.exit(0)

    if len(sys.argv) >= 4:
        residue_names = sys.argv[3:]
        residue_names.extend(["GLU", "GLY", "PHE", "LEU", "SER", "LYS", "CYS", "TRP", "TYR", "PRO",
                              "HIS", "GLN", "ARG", "ILE", "MET", "THR", "ASN", "ASP", "VAL", "ALA",
                              "A  ", "U  ", "G  ", "C  ", "DA ", "DT ", "DG ", "DC "]);
        residues = read_residues(sys.argv[1], residue_names)
    else:
        residues = read_residues(sys.argv[1])

    write_elements(sys.argv[2], residues)
