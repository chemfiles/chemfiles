# -* coding: utf-8 -*

"""
This module tries to generate the a Fortran interface declaration for the
functions it finds in a C header. It only handle edge cases for the chemharp.h
header.
"""

from pycparser import c_ast
from .constants import BEGINING, FTYPES, STRING_LENGTH
from .ctype import StringType

BEGINING += "interface\n"
END = "end interface\n"

TEMPLATE = """! Function "{cname}", at {coord}
function {name}({args}) bind(C, name="{cname}")
    use iso_c_binding
    implicit none
    {rettype} :: {name}
{declarations}
end function\n
"""


def interface(function):
    args = function.args_str()
    if function.rettype.ptr:
        rettype = "type(c_ptr)"
    else:
        rettype = "integer(c_int)"

    declarations = "\n".join([arg.to_fortran(cdef=True)
                              for arg in function.args])
    return TEMPLATE.format(name=function.c_interface_name,
                           cname=function.name,
                           args=args,
                           coord=function.coord,
                           rettype=rettype,
                           declarations=declarations)


def write_cdef(filename, functions):
    with open(filename, "w") as fd:
        fd.write(BEGINING)
        for func in functions:
            fd.write(interface(func))
        fd.write(END)
