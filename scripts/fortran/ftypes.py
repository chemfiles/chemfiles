# -* coding: utf-8 -*

"""
This module generate Chemharp main (fortran) types and bind specific
functions to them, using the name of the function.
"""
from fortran.constants import BEGINING, FTYPES

FUNCTIONS = { name:[] for name in FTYPES}
FUNCTIONS["trajectory"] = [
    'chrp_open', 'chrp_read_step', 'chrp_read_next_step',
    'chrp_write_step', 'chrp_close'
]
FUNCTIONS["free"] = [
    'chrp_strerror', 'chrp_last_error', 'chrp_loglevel', 'chrp_logfile',
    'chrp_log_stderr'
]

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
    traj = Type("trajectory")
    for proc in FUNCTIONS["trajectory"]:
        traj.add_procedure(BoundProcedure(proc[5:], proc))
    types = [traj]

    fnames = [f.name for f in functions]
    members = {}
    for name in fnames:
        if name in FUNCTIONS["free"] or name in FUNCTIONS["trajectory"]:
            continue
        tmp = name[5:]
        typename = tmp.split('_')[0]
        assert(typename in FTYPES)
        func = '_'.join(tmp.split('_')[1:])
        if not func:
            continue
        try:
            members[typename].append(func)
        except KeyError:
            members[typename] = [func]

    for typename, functions in members.items():
        t = Type(typename)
        for func in functions:
            t.add_procedure(BoundProcedure(func, "chrp_" + func))
        types.append(t)

    with open(path, "w") as fd:
        fd.write(BEGINING)
        for t in types:
            fd.write(str(t))
