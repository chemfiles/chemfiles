# -* coding: utf-8 -*

"""
Representing C types and rendering them to Fortran
"""

# Converting ANSI-C types to fortran
C_TO_F = {
    "float": "real(kind=c_float)",
    "double": "real(kind=c_double)",
    "size_t": "integer(kind=c_size_t)",
    "int": "integer(kind=c_int)",
    "bool": "logical(kind=c_bool)",
    # Enums wrapped to Fortran
    "chrp_cell_type_t":
            'include "generated/cenums.f90"\n    integer(kind(cell_type))',
    "chrp_log_level_t":
            'include "generated/cenums.f90"\n    integer(kind(log_level))',
}

# Converting Chemharp types for the fortran interface
CHRP_TYPES_TO_FORTRAN_INTERFACE = {
    "CHRP_ATOM": "class(chrp_atom)",
    "CHRP_TRAJECTORY": "class(chrp_trajectory)",
    "CHRP_FRAME": "class(chrp_frame)",
    "CHRP_CELL": "class(chrp_cell)",
    "CHRP_TOPOLOGY": "class(chrp_topology)",
}

# Converting Chemharp types for the c functions declarations
CHRP_TYPES_TO_C_DECLARATIONS = {
    "CHRP_ATOM": "type(c_ptr)",
    "CHRP_TRAJECTORY": "type(c_ptr)",
    "CHRP_FRAME": "type(c_ptr)",
    "CHRP_CELL": "type(c_ptr)",
    "CHRP_TOPOLOGY": "type(c_ptr)",
}


class CType(object):
    '''
    Representing a C type, in a simple way
    '''

    def __init__(self, cname, ptr=False, const=False):
        self.cname = cname
        self.ptr = ptr
        self.const = const
        self.CONVERSIONS = C_TO_F

    @property
    def fname(self):
        '''
        Get the fortran type name.
        '''
        return self.CONVERSIONS[self.cname]

    def to_fortran(self, cdef=False, interface=False):
        '''
        Render the type declaration in fortran
        '''
        if cdef:
            self.CONVERSIONS.update(CHRP_TYPES_TO_C_DECLARATIONS)
        elif interface:
            self.CONVERSIONS.update(CHRP_TYPES_TO_FORTRAN_INTERFACE)

        res = "    " + self.fname
        if not self.ptr:
            res += ", value"
        if self.const:
            res += ", intent(in)"

        # reset the conversions
        self.CONVERSIONS = C_TO_F
        return res


class StringType(CType):
    '''
    Representing a C string type
    '''

    def to_fortran(self, *args, **kwargs):
        if kwargs.get("cdef", False):
            self.CONVERSIONS["char"] = "character(len=1, kind=c_char), dimension(:)"
        elif kwargs.get("interface", False):
            self.CONVERSIONS["char"] = "character(len=*)"
        else:
            raise ValueError
        return super(StringType, self).to_fortran(*args, **kwargs)


class ArrayType(CType):
    '''
    Representing array argument, with various dimensions
    '''

    def __init__(self, *args, **kwargs):
        super(ArrayType, self).__init__(*args, **kwargs)
        # Do we have a compile-time unknown dimension here ?
        self.unknown = False
        self.all_dims = []
        self.ptr = True

    def dims(self, *all_dims):
        '''
        Set the array dimensions. For example, the C declaration
            int (*bar)[3]
        should be rendered in fortran as
            integer, dimension(:, :) :: bar
        by calling in this function as
            array_type.dims(-1, 3)

        The -1 value indicate a runtime size of the dimension
        '''
        for dim_size in all_dims:
            self.all_dims.append(dim_size)
            if dim_size == -1:
                self.unknown = True

    def to_fortran(self, *args, **kwargs):
        res = super(ArrayType, self).to_fortran(*args, **kwargs)
        res += ", dimension("
        if self.unknown:
            res += ", ".join([":" for i in range(len(self.all_dims))])
        else:
            res += ", ".join(map(str, self.all_dims))
        res += ")"

        return res
