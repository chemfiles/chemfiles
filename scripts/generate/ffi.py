# -* coding: utf-8 -*
import os
from pycparser import parse_file

from .functions import FunctionVisitor
from .enums import EnumsVisitor

LIBC_PATH = os.path.join(os.path.dirname(__file__), "include")


class FFI:
    '''Representing a full Foreign Function Interface in Python'''

    def __init__(self, headers, defines=None, includes=None):
        '''Create a new FFI from a set of headers'''
        if defines:
            definitions = ["-D" + d[0] + "=" + d[1] for d in defines]
        else:
            definitions = []

        includes_path = ["-I" + LIBC_PATH]
        if includes:
            includes_path.extend(["-I" + path for path in includes])

        self.enums = []
        self.functions = []
        cpp_args = ["-E"]
        cpp_args.extend(includes_path)
        cpp_args.extend(definitions)
        for header in headers:
            ast = parse_file(header,
                             use_cpp=True,
                             cpp_path="gcc",
                             cpp_args=cpp_args)
            self.enums.extend(EnumsVisitor().visit(ast))
            self.functions.extend(FunctionVisitor().visit(ast))
