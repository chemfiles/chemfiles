Supported formats
=================

List of supported formats
-------------------------

This page list the supported formats in chemfiles, with a link to the format
specification. For each format, it is also indicated which kind of data can be
read or written with |yes| for 'yes' and |no| for 'no'. Even if chemfiles can
read some type of data from a file, this does not mean that all files of this
format will contain this type of data.

The *Extension* column gives the extension used when trying to detect the
format. If your file does not have this extension, you can also use the string
in the *Format* column as a parameter to the :cpp:class:`chemfiles::Trajectory`
constructor to manually specify which format to use.

.. table::
    :class: formats-table

    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | Format          | Extension  | Read  | Write | Positions | Velocities | UnitCell  | Atoms | Bonds | Residues |
    +=================+============+=======+=======+===========+============+===========+=======+=======+==========+
    | `Amber NetCDF`_ | .nc        | |yes| | |yes| | |yes|     | |yes|      | |yes|     | |no|  | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `CIF`_          | .cif       | |yes| | |yes| | |yes|     | |no|       | |yes|     | |yes| | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `CML`_          | .cml       | |yes| | |yes| | |yes|     | |yes|      | |yes|     | |yes| | |yes| | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `CSSR`_         | .cssr      | |yes| | |yes| | |yes|     | |no|       | |yes|     | |yes| | |yes| | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `DCD`_          | .dcd       | |yes| | |no|  | |yes|     | |no|       | |no|      | |no|  | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `GRO`_          | .gro       | |yes| | |yes| | |yes|     | |yes|      | |yes|     | |yes| | |no|  | |yes|    |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `LAMMPS`_       | .lammpstrj | |yes| | |no|  | |yes|     | |no|       | |no|      | |no|  | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `LAMMPS Data`_  | |no|       | |yes| | |yes| | |yes|     | |yes|      | |yes|     | |yes| | |yes| | |yes|    |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `mmCIF`_        | .mmcif     | |yes| | |yes| | |yes|     | |no|       | |yes|     | |yes| | |yes| | |yes|    |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `MMTF`_         | .mmtf      | |yes| | |yes| | |yes|     | |no|       | |yes|     | |yes| | |yes| | |yes|    |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `MOL2`_         | .mol2      | |yes| | |yes| | |yes|     | |no|       | |yes|     | |yes| | |yes| | |yes|    |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `Molden`_       | .molen     | |yes| | |no|  | |yes|     | |no|       | |no|      | |yes| | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `PDB`_          | .pdb       | |yes| | |yes| | |yes|     | |no|       | |yes|     | |yes| | |yes| | |yes|    |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `SDF`_          | .sdf       | |yes| | |yes| | |yes|     | |no|       | |no|      | |yes| | |yes| | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `SMI`_          | .smi       | |yes| | |yes| | |no|      | |no|       | |no|      | |yes| | |yes| | |yes|    |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `TNG`_          | .tng       | |yes| | |no|  | |yes|     | |yes|      | |yes|     | |yes| | |yes| | |yes|    |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `TRJ`_          | .trj       | |yes| | |no|  | |yes|     | |no|       | |no|      | |no|  | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `TRR`_          | .trr       | |yes| | |yes| | |yes|     | |yes|      | |yes|     | |no|  | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `Tinker`_       | .arc       | |yes| | |yes| | |yes|     | |no|       | |yes|     | |yes| | |yes| | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `XTC`_          | .xtc       | |yes| | |yes| | |yes|     | |no|       | |yes|     | |no|  | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+
    | `XYZ`_          | .xyz       | |yes| | |yes| | |yes|     | |no|       | |yes|     | |yes| | |no|  | |no|     |
    +-----------------+------------+-------+-------+-----------+------------+-----------+-------+-------+----------+

- **LAMMPS** format corresponds to trajectory files written by the LAMMPS `dump
  <http://lammps.sandia.gov/doc/dump.html>`_ command.
- **LAMMPS Data** format corresponds to LAMMPS data files, as read by the LAMMPS
  `read_data <http://lammps.sandia.gov/doc/read_data.html>`_ command.

.. _Amber NetCDF: http://ambermd.org/netcdf/nctraj.xhtml
.. _CIF: https://www.iucr.org/resources/cif
.. _CML: http://www.xml-cml.org
.. _CSSR: http://www.chem.cmu.edu/courses/09-560/docs/msi/modenv/D_Files.html#944777
.. _DCD: http://www.ks.uiuc.edu/Research/vmd/plugins/molfile/dcdplugin.html
.. _GRO: http://manual.gromacs.org/current/online/gro.html
.. _LAMMPS Data: http://lammps.sandia.gov/doc/read_data.html
.. _LAMMPS: https://lammps.sandia.gov/doc/dump.html
.. _mmCIF: http://mmcif.wwpdb.org/
.. _MMTF: https://mmtf.rcsb.org/
.. _MOL2: http://chemyang.ccnu.edu.cn/ccb/server/AIMMS/mol2.pdf
.. _Molden: http://www.cmbi.ru.nl/molden/molden_format.html
.. _PDB: http://www.rcsb.org/pdb/static.do?p=file_formats/pdb/index.html
.. _SDF: http://accelrys.com/products/collaborative-science/biovia-draw/ctfile-no-fee.html
.. _SMI: http://opensmiles.org/
.. _TNG: http://doi.wiley.com/10.1002/jcc.23495
.. _TRJ: http://manual.gromacs.org/archive/5.0.7/online/trj.html
.. _TRR: http://manual.gromacs.org/current/reference-manual/file-formats.html?#trr
.. _Tinker: http://chembytes.wikidot.com/tnk-tut00#toc2
.. _XTC: http://manual.gromacs.org/current/reference-manual/file-formats.html?#xtc
.. _XYZ: https://openbabel.org/wiki/XYZ

.. role:: red

.. |yes| replace:: `✓`
.. |no| replace:: :red:`✗`

Additionally, some formats support reading and writing directly to memory,
without going through a file. At this time, all text based files (excluding
those backed by the Molfiles plugin) support both reading and writing directly
to memory. The MMTF format supports reading from a memory buffer, but does not
support writing. It is also possible to read a compressed GZ or XZ file directly
to memory buffer, but writing compressed files is not supported.

Asking for a new format
-----------------------

If you want to use chemfiles with a format which is not yet implemented, you may
easily add it by yourself if you know some C++. See the ``src/formats/XYZ.cpp``
file for example. The list of planned formats can be found `here
<gh-new-format_>`_. If you can not find your favorite format in this list, you
can `open an issue <gh-new-issue_>`_ with a description of the format, or even
better a link to the format specification.

.. _gh-new-format: https://github.com/chemfiles/chemfiles/labels/A-formats
.. _gh-new-issue: https://github.com/chemfiles/chemfiles/issues/new
.. _vmd-formats: http://www.ks.uiuc.edu/Research/vmd/plugins/molfile/
