# -* coding: utf-8 -*

"""
This module generate Chemharp Fortran API by calling the C interfae
"""
from .constants import BEGINING, FTYPES, STRING_LENGTH
from .functions import SPECIAL_FUNCTIONS, Argument
from .ctype import CType, StringType

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
    character(len={str_len}) :: string
    type(c_ptr) :: c_string

    c_string = {cname}({args})
    string = c_to_f_str(c_string)
end function
"""

COPY_RETURN_STATUS = """
    if (present(status)) then
        status = status_tmp_
    end if"""


def call_interface(args):
    '''
    Translate the arguments from fortran to C in function call
    '''
    f_types = [type_[5:] for type_ in FTYPES]
    f_types.append("this")

    args_call = [arg + "%ptr" * (arg in f_types) for arg in args.split(", ")]
    interface = "(" + ", ".join(args_call) + ")"
    return interface


def write_interface(path, _functions):
    '''
    Generate fortran subroutines corresponding to the C functions
    '''
    # Create a local copy of the functions list
    functions = _functions[:]

    for function in functions:
        # If the function is a constructor, prepend the "this" argument in the
        # arguments list
        if function.is_constructor:
            type_ = CType(function.rettype.cname, ptr=True)
            new_arg = Argument("this", type_)
            function.args = [new_arg] + function.args
            function.fname = function.name + "_init_"

        # Replace the first argument name by "this" if it is one of the
        # Chemharp types.
        try:
            arg = function.args[0]
            typename = arg.type.cname
            if typename.startswith("CHRP_"):
                arg.name = "this"
        except IndexError:
            pass

    with open(path, "w") as fd:
        fd.write(BEGINING)
        for function in functions:
            declarations = "\n".join([arg.to_fortran(interface=True)
                                     for arg in function.args])

            if isinstance(function.rettype, StringType):
                fd.write(TEMPLATE_STR_FUNCTIONS.format(
                            name=function.name,
                            cname=function.c_interface_name,
                            args=function.args_str(),
                            declarations=declarations,
                            str_len=STRING_LENGTH))
            else:
                instructions = ""
                args = function.args_str()

                if function.is_constructor:
                    instructions = "    this%ptr = "
                    instructions += function.c_interface_name
                    instructions += call_interface(args[6:])
                else:
                    instructions = "    status_tmp_ = "
                    instructions += function.c_interface_name
                    instructions += call_interface(args)
                    args = args + ", status" if args else "status"
                    declarations += "\n    integer, optional :: status"
                    declarations += "\n    integer :: status_tmp_"
                    instructions += COPY_RETURN_STATUS

                fd.write(TEMPLATE.format(
                            name=function.fname,
                            args=args,
                            declarations=declarations,
                            instructions=instructions))
