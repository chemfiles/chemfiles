#!/usr/bin/env python
# -* coding: utf-8 -*

import sys
import os

from fortran.cenums import write_enums, EnumVisitor
from fortran.cdef import write_cdef, FunctionVisitor
from fortran.ftypes import write_types
from pycparser import parse_file


def usage():
    print(sys.argv[0] + " path/to/chemharp.h path/to/fortran/binding/dir")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()
        sys.exit(0)
    FORTRAN_ROOT = sys.argv[2]
    libc_path = os.path.join(os.path.dirname(__file__), "libc")
    ast = parse_file(sys.argv[1], use_cpp=True,
                     cpp_path="gcc", cpp_args=["-E", "-I" + libc_path])

    evisitor = EnumVisitor()
    evisitor.visit(ast)

    write_enums(os.path.join(FORTRAN_ROOT, "cenums.f90"), evisitor.enums)

    fvisitor = FunctionVisitor()
    fvisitor.visit(ast)

    write_cdef(os.path.join(FORTRAN_ROOT, "cdef.f90"), fvisitor.functions)
