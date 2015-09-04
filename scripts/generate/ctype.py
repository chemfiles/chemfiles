# -* coding: utf-8 -*
"""
Representing C types
"""


class CType(object):
    '''Representing a C type, in a simple way'''

    def __init__(self, cname, is_ptr=False, is_const=False):
        self.cname = cname
        self.is_ptr = is_ptr
        self.is_const = is_const

    def __str__(self):
        return self.cname

    def __repr__(self):
        res = "const " if self.is_const else ""
        res += self.cname
        res += "*" if self.is_ptr else ""
        return res


class StringType(CType):
    '''Representing a C string type'''
    pass


class ArrayType(CType):
    '''Representing array type, with various dimensions'''

    def __init__(self, *args, **kwargs):
        super(ArrayType, self).__init__(*args, **kwargs)
        # Do we have a compile-time unknown dimension here ?
        self.unknown = False
        self.all_dims = []
        self.is_ptr = True

    def set_dimensions(self, *all_dims):
        '''
        Set the array dimensions. For example, the C declaration
        `int (*bar)[3]` should be declared by calling in this function as
        `array_type.set_dimensions(-1, 3)`

        The -1 value indicate a dimension with unknown size at compile-time
        '''
        for dim_size in all_dims:
            self.all_dims.append(dim_size)
            if dim_size == -1:
                self.unknown = True
