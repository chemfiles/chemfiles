# -* coding: utf-8 -*
from pycparser import c_ast

from .ctype import *

# All the fucntions not following the 'chrp_' + typename + fucntion_name
# pattern.
SPECIAL_FUNCTIONS = {
    "free": [
        'chrp_strerror', 'chrp_last_error', 'chrp_loglevel',
        'chrp_logfile', 'chrp_log_stderr'
    ],
    "chrp_trajectory": ['chrp_open'],
}

TYPES = [
    "CHRP_TRAJECTORY", "CHRP_CELL", "CHRP_ATOM", "CHRP_FRAME", "CHRP_TOPOLOGY"
]


class Argument:
    '''
    Representing a function argument, or return type
    '''

    def __init__(self, name, _type):
        self.name = name
        self.type = _type

    def __str__(self):
        return str(self.name)

    def __repr__(self):
        return repr(self.type) + " " + str(self.name)


class Function:
    '''
    Representing a function, with a name, a return type and some arguments
    '''
    def __init__(self, name, coord, rettype):
        self.name = name
        self.coord = str(coord).split("/")[-1].split("\\")[-1]
        self.args = []
        self.rettype = rettype

    def __str__(self):
        return self.name

    def __repr__(self):
        res = repr(self.rettype)
        res += " " + self.name + "("
        res += ", ".join(map(repr, self.args))
        res += ")"
        return res

    def add_arg(self, arg):
        '''Add an argument to the list of arguments'''
        self.args.append(arg)

    def args_str(self):
        '''Get a comma-separated string containing the argument names'''
        return ", ".join(map(str, self.args))

    @property
    def typename(self):
        '''Get the class associated with this function, or None in case of free
        functions.'''
        typename = "_".join(self.name.split("_")[:2]).upper()
        if typename in TYPES:
            return typename
        else:
            return None

    @property
    def member_name(self):
        '''
        Get the member part in the name of this function. For example, in
        "chrp_trajectory_topology_set" the member name part is "topology_set"

        In case of default constructor ("chrp_frame" for CHRP_FRAME), returns
        None
        '''
        typename = self.typename
        assert(typename)
        if typename == self.name:
            return None
        else:
            return self.name[len(typename) + 1:]

    @property
    def is_constructor(self):
        '''
        True if this function return a pointer to one of the chemharp types
        '''
        return self.rettype.cname.startswith("CHRP_")


class FunctionVisitor(c_ast.NodeVisitor):
    '''AST visitor for C function declaration.'''

    def __init__(self, *args, **kwargs):
        super(FunctionVisitor, self).__init__(*args, **kwargs)
        self.functions = []

    def visit(self, *args, **kwargs):
        super(FunctionVisitor, self).visit(*args, **kwargs)
        return self.functions

    def visit_FuncDecl(self, node):
        rettype = type_factory(node.type)
        if hasattr(node.type, "declname"):
            func = Function(node.type.declname, node.coord, rettype)
        else:
            func = Function(node.type.type.declname, node.coord, rettype)

        try:
            parameters = node.children()[0][1].params
        except AttributeError:  # No parameter for this function
            parameters = []

        for parameter in parameters:
            pa_type = type_factory(parameter.type)
            func.add_arg(Argument(parameter.name, pa_type))

        self.functions.append(func)


def type_factory(typ):
    '''
    Create a CType instance from the pycparser AST
    '''
    is_ptr = isinstance(typ, c_ast.PtrDecl)
    if is_ptr:
        if isinstance(typ.type, c_ast.ArrayDecl):
            array_decl = typ.type
            is_const = "const" in array_decl.type.quals
            name = array_decl.type.type.names[0]
            rettype = ArrayType(name, is_ptr=is_ptr, is_const=is_const)
            rettype.set_dimensions(-1, array_decl.dim.value)
        else:
            is_const = "const" in typ.type.quals
            name = typ.type.type.names[0]
            if name == "char":
                rettype = StringType(name, is_ptr=is_ptr, is_const=is_const)
            else:
                rettype = CType(name, is_ptr=is_ptr, is_const=is_const)
    else:
        if isinstance(typ.type, c_ast.ArrayDecl):
            array_decl = typ
            is_const = "const" in array_decl.type.type.quals
            name = array_decl.type.type.type.names[0]
            rettype = ArrayType(name, is_ptr=is_ptr, is_const=is_const)
            rettype.set_dimensions(array_decl.dim.value,
                                   array_decl.type.dim.value)
        else:
            name = typ.type.names[0]
            is_const = "const" in typ.quals
            rettype = CType(name, is_ptr=is_ptr, is_const=is_const)
    return rettype
