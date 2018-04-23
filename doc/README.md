# Chemfiles documentation

This directory contains the documentation for the chemfiles library. This
documentation is divided in multiple sections:

- High level overview;
- C++ interface reference;
- C interface reference;
- Developers documentation.

The documentation is written using [sphinx-doc] and [RestructuredText]. It is
automatically compiled every time a modification is added on Github, and
deployed at http://chemfiles.org/chemfiles/.

Part of the documentation (the interface reference) is extracted from the source
code. The source code contains special comments following [Doxygen] conventions,
which are extracted and included in the documentation using [breathe].

To modify the documentation, you will need to modify either the `.rst` files in
the `doc/src` directory; or the [Doxygen] comments in header files from the
`include` directory.

## Building the documentation locally

First, get the source code of chemfiles (if you don't already have it):

```
git clone https://github.com/chemfiles/chemfiles
cd chemfiles
```

Then install [Doxygen] and Python using your favorite method. Finally, install
the required python packages by running:

```
pip install sphinx
pip install -r doc/requirements.txt
```

You can now build the documentation locally by running:

```
mkdir build
cd build
cmake -DCHFL_BUILD_DOCUMENTATION=ON ..
make doc_html
```

The documentation will be in the `build/doc/html` directory.

[sphinx-doc]: http://www.sphinx-doc.org/
[RestructuredText]: http://www.sphinx-doc.org/en/stable/rest.html
[Doxygen]: http://doxygen.org/
[breathe]: http://breathe.readthedocs.io/
