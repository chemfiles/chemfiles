#!/usr/bin/env python
# -* coding: utf-8 -*
import os
from generate import FFI
from generate import fortran
from generate import python

ROOT_DIR = os.path.join(os.path.dirname(__file__), "..")
HEADER = os.path.join(ROOT_DIR, "bindings", "c", "chemharp.h")
CXX_INCLUDES = os.path.join(ROOT_DIR, "include")

FORTRAN_ROOT = os.path.join(ROOT_DIR, "bindings", "fortran", "generated")
PYTHON_ROOT = os.path.join(ROOT_DIR, "bindings", "python", "chemharp")


def generate_fortran(root):
    ffi = FFI([HEADER], includes=[CXX_INCLUDES], defines=[("CHRP_EXPORT", "")])

    fortran.write_enums(os.path.join(root, "cenums.f90"), ffi.enums)
    fortran.write_cdef(os.path.join(root, "cdef.f90"), ffi.functions)

    fortran.write_types(os.path.join(root, "ftypes.f90"), ffi.functions)
    fortran.write_interface(
        os.path.join(root, "interface.f90"),
        ffi.functions
    )


def generate_python(root):
    ffi = FFI([HEADER], includes=[CXX_INCLUDES], defines=[("CHRP_EXPORT", "")])
    python.write_ffi(os.path.join(root, "ffi.py"), ffi.enums, ffi.functions)

if __name__ == "__main__":
    generate_fortran(FORTRAN_ROOT)
    generate_python(PYTHON_ROOT)
