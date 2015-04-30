# -* coding: utf-8 -*

"""
This module parse the C header and create the list of functions
"""

from pycparser import c_ast

from .functions import Function, Argument
from .ctype import CType, StringType, ArrayType


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


def type_factory(type_):
    '''
    Create a CType instance from the pycparser AST
    '''
    is_ptr = isinstance(type_, c_ast.PtrDecl)
    if is_ptr:
        if isinstance(type_.type, c_ast.ArrayDecl):
            array_decl = type_.type
            is_const = "const" in array_decl.type.quals
            name = array_decl.type.type.names[0]
            rettype = ArrayType(name, ptr=is_ptr, const=is_const)
            rettype.dims(-1, array_decl.dim.value)
        else:
            is_const = "const" in type_.type.quals
            name = type_.type.type.names[0]
            if name == "char":
                rettype = StringType(name, ptr=is_ptr, const=is_const)
            else:
                rettype = CType(name, ptr=is_ptr, const=is_const)
    else:
        if isinstance(type_.type, c_ast.ArrayDecl):
            array_decl = type_
            is_const = "const" in array_decl.type.type.quals
            name = array_decl.type.type.type.names[0]
            rettype = ArrayType(name, ptr=is_ptr, const=is_const)
            rettype.dims(array_decl.dim.value, array_decl.type.dim.value)
        else:
            name = type_.type.names[0]
            is_const = "const" in type_.quals
            rettype = CType(name, ptr=is_ptr, const=is_const)
    return rettype
