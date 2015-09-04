# -* coding: utf-8 -*

"""
This module generate the a Fortran declaration for the C enums.
"""
from .constants import BEGINING
from .convert import enum_to_fortran

TEMPLATE = """
enum, bind(C)
{enumerators}
end enum
"""


def write_enums(path, enums):
    '''
    Generate the enum wrapping to Fortran
    '''
    with open(path, "w") as fd:
        fd.write(BEGINING)
        for enum in enums:
            fd.write(TEMPLATE.format(enumerators=enum_to_fortran(enum)))
