# -* coding: utf-8 -*

"""
This module generate Chemharp main (fortran) types and bind specific
functions to them, using the name of the function.
"""
from fortran.constants import BEGINING
from fortran.functions import members_functions, FUNCTIONS

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
    members = members_functions(functions)

    traj = Type("trajectory")
    for proc in FUNCTIONS["trajectory"]:
        traj.add_procedure(BoundProcedure(proc[5:], proc))
    types = [traj]

    for typename, functions in members.items():
        t = Type(typename)
        t.add_procedure(BoundProcedure("init", "chrp_" + typename))
        for func in functions:
            t.add_procedure(BoundProcedure(func, "chrp_" + typename + "_" + func))
        types.append(t)

    with open(path, "w") as fd:
        fd.write(BEGINING)
        for t in types:
            fd.write(str(t))
