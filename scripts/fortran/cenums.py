# -* coding: utf-8 -*

"""
This module generate wrappers for the enums in the C interface
"""
from .constants import BEGINING
from pycparser import c_ast

TEMPLATE = """
enum, bind(C)
{enumerators}
end enum
"""

C2F_ENUMS = {
    "CHRP_LOG_LEVEL": "log_level",
    "CHRP_CELL_TYPES": "cell_type",
}


class Enum:
    '''Class representing a C enum'''

    def __init__(self, name):
        self.name = name
        self.enumerator = []

    def append(self, name, value=None):
        '''
        Append a name in the enum list, with optional value
        '''
        self.enumerator.append((name, value))

    def __str__(self):
        res = ""
        for name, value in self.enumerator:
            res += "    enumerator :: " + name
            if value is not None:
                res += " = " + value
            res += "\n"
        res += "    ! Enumeration name:\n"
        res += "    enumerator :: " + self.name
        return res


class EnumVisitor(c_ast.NodeVisitor):
    '''AST visitor for C enum declaration.'''

    def __init__(self, *args, **kwargs):
        super(EnumVisitor, self).__init__(*args, **kwargs)
        self.enums = []

    def visit_Enum(self, node):
        enum = Enum(C2F_ENUMS[node.name])
        for enumerator in node.values.enumerators:
            enum.append(enumerator.name, enumerator.value)
        self.enums.append(enum)


def write_enums(path, enums):
    '''
    Generate the enum wrapping to Fortran
    '''
    with open(path, "w") as fd:
        fd.write(BEGINING)
        for enum in enums:
            fd.write(TEMPLATE.format(enumerators=str(enum)))
