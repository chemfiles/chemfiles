# -* coding: utf-8 -*

"""
This module associate functions names and types
"""
from pycparser import c_ast
from fortran.constants import FTYPES

FUNCTIONS = {
    "trajectory": [
        'chrp_open', 'chrp_read_step', 'chrp_read_next_step',
        'chrp_write_step', 'chrp_close'
    ],
    "free": [
        'chrp_strerror', 'chrp_last_error', 'chrp_loglevel',
        'chrp_logfile', 'chrp_log_stderr'
    ],
    "atom": ['chrp_topology_atom']
}


def members_functions(functions):
    '''
    Guess type <--> functions associations from the function names
    '''
    fnames = [f.name for f in functions]
    members = {}

    ignored = []
    for names in FUNCTIONS.values():
        ignored.extend(names)

    for name in fnames:
        if name in ignored:
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
    return members


class Type:

    def __init__(self, typename):
        self.typename = typename

    def children(self):
        return []


class TypeVisitor(c_ast.NodeVisitor):
    '''AST visitor for C function arguments'''

    def __init__(self, C_TO_F, CHRP_TO_F, target="iso_c_binding"):
        super(TypeVisitor, self).__init__()
        self.C_TO_F = C_TO_F
        self.CHRP_TO_F = CHRP_TO_F
        self.target = target

    def visit(self, *args, **kwargs):
        self.declaration = "    "
        super(TypeVisitor, self).visit(*args, **kwargs)
        return self.declaration

    def visit_TypeDecl(self, node):
        self.declaration += self.C_TO_F[node.type.names[0]]
        if self.target == "iso_c_binding":
            self.declaration += ", value"

    def visit_Type(self, node):
        self.declaration += "class(" + node.typename + ")"

    def visit_PtrDecl(self, node):
        if isinstance(node.type, c_ast.ArrayDecl):
            typename = self.C_TO_F[node.type.type.type.names[0]]
            self.declaration += typename
            self.declaration += ", dimension(:, :)"
        else:
            typename = node.type.type.names[0]
            if typename in self.CHRP_TO_F.keys():
                typename = self.CHRP_TO_F[typename]
            elif typename == "char" and self.target == "iso_c_binding":
                typename = "character(len=1, kind=c_char), dimension(:)"
            elif typename == "char" and self.target == "fortran":
                typename = "character(len=*)"
            else:
                typename = self.C_TO_F[typename]

            self.declaration += typename
            if "const" in node.type.quals:
                self.declaration += ", intent(in)"

    def visit_ArrayDecl(self, node, dims=[]):
        self.declaration += self.C_TO_F[node.type.type.type.names[0]]
        self.declaration += ", dimension(3, 3)"
