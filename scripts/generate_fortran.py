#!/usr/bin/env python
# -* coding: utf-8 -*

import sys
import os

from fortran.parse import FunctionVisitor
from fortran.cenums import write_enums, EnumVisitor
from fortran.cdef import write_cdef
from fortran.ftypes import write_types
from fortran.interface import write_interface
from pycparser import parse_file

ROOT_DIR = os.path.join(os.path.dirname(__file__), "..")
C_HEADER = os.path.join(ROOT_DIR, "bindings", "c", "chemharp.h")
CXX_INCLUDES = os.path.join(ROOT_DIR, "include")
FORTRAN_ROOT = os.path.join(ROOT_DIR, "bindings", "fortran", "generated")
LIBC_PATH = os.path.join(os.path.dirname(__file__), "libc")

if __name__ == "__main__":
    ast = parse_file(C_HEADER, use_cpp=True, cpp_path="gcc",
                     cpp_args=["-E", "-I" + LIBC_PATH, "-I" + CXX_INCLUDES])

    evisitor = EnumVisitor()
    evisitor.visit(ast)

    write_enums(os.path.join(FORTRAN_ROOT, "cenums.f90"), evisitor.enums)

    fvisitor = FunctionVisitor()
    functions = fvisitor.visit(ast)

    write_cdef(os.path.join(FORTRAN_ROOT, "cdef.f90"), functions)
    write_types(os.path.join(FORTRAN_ROOT, "ftypes.f90"), functions)
    write_interface(os.path.join(FORTRAN_ROOT, "interface.f90"), functions)
