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

class Interner:
    def __init__(self):
        self.list = []

    def get(self, string):
        for (i, known) in enumerate(self.list):
            if string == known:
                return i
        self.list.append(string)
        return len(self.list) - 1


INTERNER = Interner()


def join_and_wrap_80(indent, list):
    result = ""
    line = indent
    for string in list:
        line += string + ", "
        if len(line) > 72:
            result += line + "\n"
            line = indent
    if line != indent:
        result += line
    return result


class Residue:
    def __init__(self, code):
        self.code = code.strip()
        self.atoms = {}

    def __str__(self):
        connectivity = []
        for atom, connections in self.atoms.iteritems():
            for connected in connections:
                connectivity.append('{{{}, {}}}'.format(atom, connected))
        connectivity = join_and_wrap_80("        ", connectivity)
        return '{{"{}", {{\n{}    }}}}'.format(self.code, connectivity)

    def add_atom(self, name, connected):
        self.atoms[INTERNER.get(name)] = [INTERNER.get(s) for s in connected]

def read_residues(path, accepted_residues=None):
    residues = []
    read_residue = False

    with open(path) as f:
        for line in f:
            line = line.replace(r'"', r'\"')
            if line[:7] == "RESIDUE":
                read_residue = False
                if accepted_residues is None or line[10:13] in accepted_residues:
                    residues.append(Residue(line[10:13]))
                    read_residue = True
            if line[:6] == "CONECT" and read_residue:
                splitted = re.split(r'\s+', line)[1:-1]
                residues[-1].add_atom(splitted[0], splitted[2:])

    return residues

HEADER = """// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

// !!!! AUTO-GENERATED FILE !!!! Do not edit. See pdb_connectivity.py for the code.
// The data comes from the PDB's connectivity table:
// http://ftp.wwpdb.org/pub/pdb/data/monomers/het_dictionary.txt

#include "chemfiles/pdb_connectivity.hpp"
using namespace chemfiles;

"""

def write_elements(path, residues):
    with open(path, "w") as fd:
        fd.write(HEADER)

        fd.write("// generated for the following residues\n")
        fd.write(join_and_wrap_80("// ", [r.code for r in residues]))
        fd.write("\n\n")

        fd.write("const std::vector<std::string> InternedName::INTERNER_ = {\n")
        fd.write(join_and_wrap_80("    ", ['"{}"'.format(name) for name in INTERNER.list]))
        fd.write("};\n\n\n")

        fd.write("const PDBConnectivity::PDBConnectMap PDBConnectivity::PDB_CONNECTIVITY_MAP_ = {\n")
        for residue in residues:
            fd.write("    " + str(residue) + ",\n")
        fd.write("};\n\n")


def usage():
    print(sys.argv[0] + " path/to/het_dictionary.txt pdb_connectivity.cpp [Residue names...]")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()
        sys.exit(0)

    if len(sys.argv) >= 4:
        residue_names = set(sys.argv[3:])
        residue_names.update({
            "GLU", "GLY", "PHE", "LEU", "SER", "LYS", "CYS", "TRP", "TYR",
            "PRO", "HIS", "GLN", "ARG", "ILE", "MET", "THR", "ASN", "ASP",
            "VAL", "ALA", "A  ", "U  ", "G  ", "C  ", "DA ", "DT ", "DG ", "DC "
        });
        residues = read_residues(sys.argv[1], residue_names)
    else:
        residues = read_residues(sys.argv[1])

    write_elements(sys.argv[2], residues)
