Supported formats
=================

List of supported formats
-------------------------

This page list the supported formats in Chemharp, with a link to the format
specification. For eavch format, it is also indicated wether they have some
information to create a *Topology*, and if you can read them, write them or both
using Chemharp.

The *Extension* column gives the extension used when trying to detect the format.
If your file does not have this extension, you can also use the string in the
*Format* column as a parameter to the ``Trajectory`` constructor to manually
specify which format to use.

+-------------------+------------+-------------------+---------+---------+
|  Format           | Extension  | Topology ?        | Read ?  | Write ? |
+===================+============+===================+=========+=========+
| `XYZ`_            | .xyz       | |yes| Atom names  | |yes|   | |yes|   |
+-------------------+------------+-------------------+---------+---------+
| `Amber NetCDF`_   | .nc        | |no|              | |yes|   | |no|    |
+-------------------+------------+-------------------+---------+---------+


.. _XYZ: http://openbabel.org/wiki/XYZ
.. _Amber NetCDF: http://ambermd.org/netcdf/nctraj.xhtml

.. |yes| image:: static/img/yes.png
          :alt: Yes
          :width: 16px
          :height: 16px

.. |no| image:: static/img/no.png
          :alt: No
          :width: 16px
          :height: 16px

Asking for a new format
-----------------------

If you want to use Chemharp with a format which is not yet implemented, you may
easily add it by yourself if you know some C++. Just see the :file:`src/formats/XYZ.cpp`
file for example.

The list of planned formats can be found `here <https://github.com/Luthaf/Chemharp/labels/New%20Format>`_.
If and only if you can not find your format in this list, you can also add
another format request at `Github <https://github.com/Luthaf/Chemharp/issues/new>`_.
In that case, please use the following header at the top of your message, and
complete everything between square braces.

.. code-block:: text

    File Type                  | [text / binary]
    ---------------------------|---------------
    Topological information    | [Yes / No]
    Positions                  | [Yes / No]
    Velocities                 | [Yes / No]

    Format specification: [link to the specification]
    File example: [link to a file example]
