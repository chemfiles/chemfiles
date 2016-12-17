Functions for errors handling
-----------------------------

Apart from the *constructor* functions (the functions returning pointers to the
types defined above); all the functions return a status code, which is 0 if
nothing went wrong, and another value in case of error. The following function
allow for error handling from the C side.

.. doxygenfunction:: chfl_strerror

.. doxygenfunction:: chfl_last_error

.. doxygenfunction:: chfl_clear_errors

.. doxygentypedef:: chfl_warning_callback

.. doxygenfunction:: chfl_set_warning_callback

The return values for this status code correspond to the following enum:

.. doxygenenum:: chfl_status
