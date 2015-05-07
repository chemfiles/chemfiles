# -* coding: utf-8 -*

"""
This module generate Chemharp main (fortran) types and bind specific
functions to them, using the name of the function.
"""
from .constants import BEGINING, FTYPES
from .functions import SPECIAL_FUNCTIONS

TEMPLATE = """
type {name}
    private
    type(c_ptr) :: ptr
contains
{procedures}
end type
"""


class BoundProcedure:
    '''Class reprensenting a bound procedure for a Fortran type'''

    def __init__(self, name, procedure):
        self.name = name
        self.procedure = procedure

    def __str__(self):
        return "procedure :: {name} => {proc}".format(name=self.name,
                                                      proc=self.procedure)


class Type:
    '''Class reprensenting a Fortran type'''

    def __init__(self, name):
        self.name = name
        self.procedures = []

    def add_procedure(self, proc):
        self.procedures.append(proc)

    def __str__(self):
        tmp = ""
        for proc in self.procedures:
            tmp += "    {proc}\n".format(proc=str(proc))
        tmp = tmp[:-1]  # Remove last \n
        return TEMPLATE.format(name=self.name, procedures=tmp)


def write_types(path, functions):
    '''
    Generate types definitions for the fortran interface.
    '''
    types = {}

    for func in functions:
        if func.name in SPECIAL_FUNCTIONS.values():
            continue
        else:
            typename = func.typename
            if typename is None:
                continue
            try:
                ftype = types[typename]
            except KeyError:
                ftype = Type(typename)
                types[typename] = ftype

            ftype.add_procedure(BoundProcedure(func.member_name, func.fname))

    # TODO: find something generic here
    types["chrp_trajectory"].add_procedure(
            BoundProcedure('open', 'chrp_open_init_')
    )

    with open(path, "w") as fd:
        fd.write(BEGINING)
        for type_ in types.values():
            fd.write(str(type_))
