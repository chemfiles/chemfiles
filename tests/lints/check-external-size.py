#!/usr/bin/env python
# -*- coding: utf8 -*-
"""
This script checks that all the external archive included in the repository are
as small as they can be.
"""
from __future__ import print_function
import os
import sys
import glob

ROOT = os.path.join(os.path.dirname(__file__), "..", "..")
ERRORS = 0


# when adding new files here, make sure that they are as small as possible!
EXPECTED_SIZES = {
    "bzip2.tar.gz": 114,
    "fmt.tar.gz": 701,
    "gemmi.tar.gz": 476,
    "lzma.tar.gz": 256,
    "mmtf-cpp.tar.gz": 439,
    "molfiles.tar.gz": 477,
    "netcdf.tar.gz": 494,
    "pugixml.tar.gz": 198,
    "tng.tar.gz": 207,
    "toml11.tar.gz": 69,
    "xdrfile.tar.gz": 26,
    "zlib.tar.gz": 370,
}


def error(message):
    global ERRORS
    ERRORS += 1
    print(message)


if __name__ == "__main__":
    for path in glob.glob(os.path.join(ROOT, "external", "*.tar.gz")):
        size = os.path.getsize(path)
        size_kb = size // 1024
        name = os.path.basename(path)

        if name not in EXPECTED_SIZES:
            error("{} is not a known external file, please edit this file".format(name))

        expected = EXPECTED_SIZES[name]
        if size_kb > 1.1 * expected:
            error("{} size increased by more than 10%".format(name))

        if size_kb < 0.7 * expected:
            error("{} size decreased by more than 30%, edit this file".format(name))

    if ERRORS != 0:
        sys.exit(1)
