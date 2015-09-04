# -* coding: utf-8 -*

"""
This module generate Chemharp Fortran API by calling the C interfae
"""
from generate.functions import SPECIAL_FUNCTIONS, Argument
from generate.ctype import *

from .constants import BEGINING, FTYPES, STRING_LENGTH
from .convert import arg_to_fortran, function_name_to_fortran

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

CHECK_NULL_POINTER = """

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if
"""


def call_interface(args):
    '''
    Translate the arguments from fortran to C in function call
    '''
    f_types = [type_[5:] for type_ in FTYPES]
    f_types.append("this")

    args_call = []
    for arg in args:
        if isinstance(arg.type, StringType) and arg.type.is_const:
            # Convert const string
            call = "f_to_c_str(" + arg.name + ")"
        elif isinstance(arg.type, ArrayType):
            # Use pointers for arrays
            call = "c_loc(" + arg.name + ")"
        else:
            call = arg.name
            if arg.name in f_types:
                call += "%ptr"
        args_call.append(call)

    interface = "(" + ", ".join(args_call) + ")"
    return interface


def post_call_processing(args):
    '''
    Post-process strings after call when needed
    '''
    res = ""
    for arg in args:
        if isinstance(arg.type, StringType) and not arg.type.is_const:
            res = arg.name + " = rm_c_null_in_str(" + arg.name + ")"
    return res


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
            type_ = CType(function.rettype.cname, is_ptr=True)
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
            declarations = "\n".join([arg_to_fortran(arg, interface=True)
                                     for arg in function.args])

            if isinstance(function.rettype, StringType):
                fd.write(TEMPLATE_STR_FUNCTIONS.format(
                    name=function.name,
                    cname=function.name + "_c",
                    args=function.args_str(),
                    declarations=declarations,
                    str_len=STRING_LENGTH))
            else:
                instructions = ""
                args = ", ".join([arg.name for arg in function.args])

                if function.is_constructor:
                    instructions = "    this%ptr = "
                    instructions += function.name + "_c"
                    instructions += call_interface(function.args[1:])
                    instructions += CHECK_NULL_POINTER
                else:
                    instructions = "    status_tmp_ = "
                    instructions += function.name + "_c"
                    instructions += call_interface(function.args)

                declarations += "\n    integer, optional :: status"
                declarations += "\n    integer :: status_tmp_"
                instructions += COPY_RETURN_STATUS

                args += ", status" if args else "status"

                post_call = post_call_processing(function.args)
                if post_call:
                    instructions += "\n    " + post_call

                fd.write(TEMPLATE.format(
                    name=function_name_to_fortran(function),
                    args=args,
                    declarations=declarations,
                    instructions=instructions))
