# -* coding: utf-8 -*

"""
This module create the python version of C arguments, types, ...
"""

from generate.ctype import *
from generate.functions import TYPES

CONVERSIONS = {
    "float": "c_float",
    "double": "c_double",
    "size_t": "c_size_t",
    "int": "c_int",
    "bool": "c_bool",

    "CHRP_ATOM": "CHRP_ATOM",
    "CHRP_TRAJECTORY": "CHRP_TRAJECTORY",
    "CHRP_FRAME": "CHRP_FRAME",
    "CHRP_CELL": "CHRP_CELL",
    "CHRP_TOPOLOGY": "CHRP_TOPOLOGY",

    "chrp_cell_type_t": "c_int",
    "chrp_log_level_t": "c_int",
}

NUMPY_CONVERSIONS = {
    "float": "float32",
    "double": "float64",
    "size_t": "uintp",
    "int": "int32",
    "bool": "bool",
}


def type_to_python(typ, cdef=False, interface=False):
    if isinstance(typ, StringType):
        return "c_char_p"
    elif isinstance(typ, ArrayType):
        return array_to_python(typ, cdef=cdef, interface=interface)
    else:
        if typ.is_ptr:
            return "POINTER(" + CONVERSIONS[typ.cname] + ")"
        else:
            return CONVERSIONS[typ.cname]


def array_to_python(typ, cdef=False, interface=False):
    ctype = NUMPY_CONVERSIONS[typ.cname]
    res = 'ndpointer(np.' + ctype + ', flags="C_CONTIGUOUS"'
    res += ', ndim=' + str(len(typ.all_dims))
    if not typ.unknown:
        res += ', shape=(' + ", ".join(map(str, typ.all_dims)) + ')'
    res += ')'
    return res
