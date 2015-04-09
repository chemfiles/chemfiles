# -* coding: utf-8 -*

"""
This module generate Chemharp Fortran API by calling the C interfae
"""
from fortran.constants import BEGINING, FTYPES
from fortran.cdef import Argument
from fortran.functions import FUNCTIONS, TypeVisitor, Type

TEMPLATE = """
subroutine {name}({args})
    implicit none
{declarations}

{instructions}
end subroutine
"""

STR_FUNCTIONS = ["chrp_strerror", "chrp_last_error"]

TEMPLATE_STR_FUNCTIONS = """
function {name}({args}) result(string)
    implicit none
{declarations}
    character, pointer, dimension(:) :: string
    type(c_ptr) :: c_string

    c_string = {name}_c({args})
    call c_f_pointer(c_string, string, [1])
end function
"""

COPY_RETURN_STATUS = """
    if (present(status)) then
        status = status_tmp_
    end if"""

CHRP_TYPES_TO_F = {
    "CHRP_ATOM": "class(atom)",
    "CHRP_TRAJECTORY": "class(trajectory)",
    "CHRP_FRAME": "class(frame)",
    "CHRP_CELL": "class(cell)",
    "CHRP_TOPOLOGY": "class(topology)",
}

C_TO_F = {
    "float": "real(kind=c_float)",
    "double": "real(kind=c_double)",
    "size_t": "integer(kind=c_size_t)",
    "int": "integer(kind=c_int)",
    "bool": "logical(kind=c_bool)",
    # Enums wrapped to Fortran
    "chrp_cell_type_t": 'include "cenums.f90"\n    integer(kind(cell_type))',
    "chrp_log_level_t": 'include "cenums.f90"\n    integer(kind(log_level))',
}


def call_interface(args):
    '''
    Translate the arguments from fortran to C in function call
    '''
    interface = "("

    f_types = [name + "_" for name in FTYPES]
    f_types.append("this")

    for arg in args.split(", "):
        if arg in f_types:
            interface += arg + "%ptr, "
        else:
            interface += arg + ", "
    interface = interface[:-2]
    interface += ")"
    return interface

def write_interface(path, functions):
    '''
    Generate fortran subroutines corresponding to the C functions
    '''

    vis = TypeVisitor(C_TO_F, CHRP_TYPES_TO_F, target="fortran")

    for function in functions:
        # If the function is a constructor, prepend the "this" argument in the
        # arguments list
        if function.is_constructor:
            T = Type(function.return_type())
            new_arg = Argument("this", T)
            function.args = [new_arg] + function.args

        # Replace the first argument name by "this" if it is one of the
        # Chemharp types.
        try:
            arg = function.args[0]
            typename = arg.type.type.type.names[0]
            if typename in CHRP_TYPES_TO_F.keys():
                arg.name = "this"
        except (IndexError, AttributeError):
            pass

        # Replace all the other occurences of a Chemharp type as argument name
        # by the argument name with a "_" suffix.
        for arg in function.args[1:]:
            try:
                typename = arg.type.type.type.names[0]
            except AttributeError:
                continue

            if typename in CHRP_TYPES_TO_F.keys():
                arg.name += "_"


    with open(path, "w") as fd:
        fd.write(BEGINING)
        for function in functions:
            declarations = ""
            instructions = ""
            for arg in function.args:
                declarations += vis.visit(arg.type)
                declarations += " :: " + arg.name + "\n"
            declarations = declarations[:-1]

            if function.name in STR_FUNCTIONS:
                fd.write(TEMPLATE_STR_FUNCTIONS.format(
                            name=function.name,
                            args=function.args_str(),
                            declarations=declarations))
            else:
                args=function.args_str()

                if function.is_constructor:
                    instructions = "    this%ptr = "
                    instructions += function.name + "_c" + call_interface(args[6:])
                else:
                    instructions = "    status_tmp_ = "
                    instructions += function.name + "_c" + call_interface(args)
                    args = args + ", status" if args else "status"
                    declarations += "\n    integer, optional :: status"
                    declarations += "\n    integer :: status_tmp_"
                    instructions += COPY_RETURN_STATUS

                fd.write(TEMPLATE.format(
                            name=function.name,
                            args=args,
                            declarations=declarations,
                            instructions=instructions))
