# -* coding: utf-8 -*

"""
This module associate functions names and types
"""
from .constants import FTYPES


class Argument:
    '''
    Representing a function argument, or return type
    '''

    def __init__(self, name, _type):
        self.name = name
        self.type = _type

    def __str__(self):
        return str(self.name)

    def to_fortran(self, *args, **kwargs):
        '''
        Render the fortran type declaration of this argument
        '''
        res = self.type.to_fortran(*args, **kwargs)
        res += " :: " + self.name
        return res


class Function:
    '''
    Representing a function, with a name, a return type and some arguments
    '''
    def __init__(self, name, coord, rettype):
        self.name = name
        self.coord = coord
        self.args = []
        self.rettype = rettype

    def add_arg(self, arg):
        '''Add an argument to the list of arguments'''
        self.args.append(arg)

    def args_str(self):
        '''Get a comma-separated string containing the argument names'''
        return ", ".join(map(str, self.args))

    @property
    def fname(self):
        '''
        Fortran function name
        '''
        name = self.name
        if self.is_constructor:
            name += "_init_"
        return name

    @property
    def c_interface_name(self):
        '''
        C-interface function name
        '''
        return self.name + "_c"

    @property
    def typename(self):
        '''
        Get the Fortran type associated with this function, or None in case of
        free functions.
        '''
        typename = "_".join(self.name.split("_")[:2])
        if typename in FTYPES:
            return typename
        else:
            return None

    @property
    def member_name(self):
        '''
        Get the member part in the name of this function. For example, in
            chrp_trajectory_topology_set
        the member name part is
            topology_set
        '''
        typename = self.typename
        assert(typename)
        if typename == self.name:
            return "init"
        else:
            return self.name[len(typename) + 1:]

    @property
    def is_constructor(self):
        '''
        True if this function return a pointer to one of the chemharp types
        '''
        return self.rettype.cname.startswith("CHRP_")


# All the fucntions not following the 'chrp_' + typename + fucntion_name
# pattern.
SPECIAL_FUNCTIONS = {
    "free": [
        'chrp_strerror', 'chrp_last_error', 'chrp_loglevel',
        'chrp_logfile', 'chrp_log_stderr'
    ],
    "chrp_trajectory": ['chrp_open'],
    "chrp_topology": ['chrp_empty_topology'],
}
