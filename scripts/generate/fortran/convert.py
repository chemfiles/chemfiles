# -* coding: utf-8 -*

"""
This module create the fortran version of C arguments, types, ...
"""

from generate.ctype import *
from generate.functions import TYPES

CONVERSIONS = {
    "float": "real(kind=c_float)",
    "double": "real(kind=c_double)",
    "size_t": "integer(kind=c_size_t)",
    "int": "integer(kind=c_int)",
    "bool": "logical(kind=c_bool)",
}

CHRP_TYPES_TO_FORTRAN_INTERFACE = {
    "CHRP_ATOM": "class(chrp_atom)",
    "CHRP_TRAJECTORY": "class(chrp_trajectory)",
    "CHRP_FRAME": "class(chrp_frame)",
    "CHRP_CELL": "class(chrp_cell)",
    "CHRP_TOPOLOGY": "class(chrp_topology)",

    # Enums wrapped to Fortran
    "chrp_cell_type_t":
    'include "generated/cenums.f90"\n    integer(kind=kind(CHRP_CELL_TYPES))',
    "chrp_log_level_t":
    'include "generated/cenums.f90"\n    integer(kind=kind(CHRP_LOG_LEVEL))',
    "chrp_atom_type_t":
    'include "generated/cenums.f90"\n    integer(kind=kind(CHRP_ATOM_TYPES))',
}

# Converting Chemharp types for the c functions declarations
CHRP_TYPES_TO_C_DECLARATIONS = {
    "CHRP_ATOM": "type(c_ptr), value",
    "CHRP_TRAJECTORY": "type(c_ptr), value",
    "CHRP_FRAME": "type(c_ptr), value",
    "CHRP_CELL": "type(c_ptr), value",
    "CHRP_TOPOLOGY": "type(c_ptr), value",

    "chrp_cell_type_t": "integer(kind=c_int)",
    "chrp_log_level_t": "integer(kind=c_int)",
    "chrp_atom_type_t": "integer(kind=c_int)",
}


def type_to_fortran(typ, cdef=False, interface=False):
    if isinstance(typ, StringType):
        return string_to_fortran(typ, cdef=cdef, interface=interface)
    elif isinstance(typ, ArrayType):
        return array_to_fortran(typ, cdef=cdef, interface=interface)
    else:
        return ctype_to_fortran(typ, cdef=cdef, interface=interface)


def ctype_to_fortran(typ, cdef=False, interface=False):
    conversions = CONVERSIONS.copy()
    if cdef:
        conversions.update(CHRP_TYPES_TO_C_DECLARATIONS)
    elif interface:
        conversions.update(CHRP_TYPES_TO_FORTRAN_INTERFACE)
    res = conversions[typ.cname]
    if not typ.is_ptr:
        res += ", value"
    if typ.is_const:
        res += ", intent(in)"
    return res


def string_to_fortran(typ, cdef=False, interface=False):
    if cdef:
        res = "character(len=1, kind=c_char), dimension(*)"
    elif interface:
        res = "character(len=*)"
    if not typ.is_ptr:
        res += ", value"
    if typ.is_const:
        res += ", intent(in)"
    return res


def array_to_fortran(typ, cdef=False, interface=False):
    if cdef:
        return "type(c_ptr), value"
    elif interface:
        res = ctype_to_fortran(typ)
        res += ", dimension("
        if typ.unknown:
            res += ", ".join([":" for i in range(len(typ.all_dims))])
        else:
            res += ", ".join(map(str, typ.all_dims))
        res += "), target"
        return res


def arg_to_fortran(argument, cdef=False, interface=False):
    res = "    " + type_to_fortran(argument.type,
                                   cdef=cdef,
                                   interface=interface)
    res += " :: " + argument.name
    return res


def enum_to_fortran(enum):
    res = ""
    for e in enum.enumerators:
        res += "    enumerator :: " + e.name
        if e.value is not None:
            res += " = " + e.value.value
        res += "\n"
    res += "    ! Enumeration name:\n"
    res += "    enumerator :: " + enum.name
    return res


def function_name_to_fortran(function):
    if function.is_constructor:
        return function.name + "_init_"
    else:
        return function.name
