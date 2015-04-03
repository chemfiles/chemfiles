# -* coding: utf-8 -*

"""
This module tries to generate the a Fortran interface declaration for the
functions it finds in a C header. It only handle edge cases for the chemharp.h
header.
"""

from pycparser import c_ast
from fortran.constants import BEGINING


class Function:
    '''Class representing a C function declaration, with a name, some arguments
    and a return type.'''

    def __init__(self, name, coord, return_ptr=False):
        self.name = name
        self.coord = coord
        self.args = []
        self.return_ptr = return_ptr

    def add_arg(self, arg):
        '''Add an argument to the list of arguments'''
        self.args.append(arg)

    def args_str(self):
        '''Get a comma-separated string containing the argument names'''
        args = ""
        for arg in self.args:
            args += str(arg) + ", "
        if args:
            args = args[:-2]
        return args


class Argument:
    '''Function argument representation, with a name, a C type, and some
    modifiers: pointer and const'''

    def __init__(self, name, typ, is_ptr=False, is_const=False):
        if name:
            self.name = name
        else:
            self.name = "void"
        self.type = typ
        self.is_ptr = is_ptr
        self.is_const = is_const

    def __str__(self):
        return self.name


class FunctionVisitor(c_ast.NodeVisitor):
    '''AST visitor for C function declaration.'''

    def __init__(self, *args, **kwargs):
        super(FunctionVisitor, self).__init__(*args, **kwargs)
        self.functions = []

    def visit_FuncDecl(self, node):
        if hasattr(node.type, "declname"):
            f = Function(node.type.declname, node.coord)
        else:
            f = Function(node.type.type.declname, node.coord, return_ptr=True)

        try:
            params = node.children()[0][1].params
        except AttributeError:  # No argument for this function
            params = []

        for pa in params:
            f.add_arg(Argument(pa.name, pa.type))

        self.functions.append(f)


BEGINING += "interface\n"
END = "end interface\n"

TEMPLATE = """! Function "{fname}", at {coord}
function {fname}_c({args}) bind(C, name="{fname}")
    use iso_c_binding
    implicit none
    {ret_type} :: {fname}_c
{declarations}
end function\n
"""

CHRP_TYPES = [
    "CHRP_ATOM", "CHRP_TRAJECTORY", "CHRP_FRAME", "CHRP_CELL", "CHRP_TOPOLOGY"
]

C2F_TYPENAMES = {
    "float": "real(kind=c_float)",
    "double": "real(kind=c_double)",
    "size_t": "integer(kind=c_size_t)",
    "int": "integer(kind=c_int)",
    "bool": "logical(kind=c_bool)",
    # Enums wrapped to Fortran
    "chrp_cell_type_t": 'include "cenums.f90"\n    integer(kind(cell_type))',
    "chrp_log_level_t": 'include "cenums.f90"\n    integer(kind(log_level))',
}


def interface(function):
    args = function.args_str()
    if function.return_ptr:
        ret_type = "type(c_ptr)"
    else:
        ret_type = "integer(c_int)"

    declarations = ""
    for arg in function.args:
        typedecl = "    "
        if isinstance(arg.type, c_ast.TypeDecl):
            typedecl += C2F_TYPENAMES[arg.type.type.names[0]]
            typedecl += ", value"
            typedecl += " :: " + arg.type.declname
        if isinstance(arg.type, c_ast.ArrayDecl):
            typedecl += C2F_TYPENAMES[arg.type.type.type.type.names[0]]
            typedecl += ", dimension(3, 3)"
            typedecl += " :: " + arg.type.type.type.declname
        elif isinstance(arg.type, c_ast.PtrDecl):
            if isinstance(arg.type.type, c_ast.ArrayDecl):
                typename = C2F_TYPENAMES[arg.type.type.type.type.names[0]]
                typedecl += typename
                typedecl += ", dimension(:, :)".format(
                                                  size=arg.type.type.dim.value)

                typedecl += " :: " + arg.type.type.type.declname
            else:
                typename = arg.type.type.type.names[0]
                if typename in CHRP_TYPES:
                    typename = "type(c_ptr)"
                elif typename == "char":
                    typename = "character(len=1, kind=c_char), dimension(:)"
                else:
                    typename = C2F_TYPENAMES[typename]

                typedecl += typename
                if "const" in arg.type.type.quals:
                    typedecl += ", intent(in)"
                typedecl += " :: " + arg.type.type.declname

        declarations += typedecl + "\n"

    declarations = declarations[:-1]  # Remove last \n
    return TEMPLATE.format(fname=function.name,
                           args=args,
                           coord=function.coord,
                           ret_type=ret_type,
                           declarations=declarations)


def write_cdef(filename, functions):
    with open(filename, "w") as fd:
        fd.write(BEGINING)
        for func in functions:
            fd.write(interface(func))
        fd.write(END)
