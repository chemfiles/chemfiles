#!/usr/bin/env python
# -*- coding: utf8 -*-
"""
This script check that only whitelisted headers are included (transitivly) by
including chemfiles.h or chemfiles.hpp.
"""
import os
import sys
import re

ROOT = os.path.join(os.path.dirname(__file__), "..", "..")
GENERATED_HEADERS = ["chemfiles/config.hpp", "chemfiles/exports.hpp"]
ERRORS = 0

WHITELIST = [
    # standard C99 headers
    "stdbool.h", "stdint.h",
    # standard C++11 headers
    "iterator", "functional", "cstdint", "array", "utility", "cassert",
    "string", "memory", "exception", "limits", "algorithm", "stdexcept",
    "vector", "cmath", "type_traits", "unordered_map", "cfloat",
    # external headers
    "chemfiles/external/span.hpp",
    "chemfiles/external/optional.hpp",
    # chemfiles helper headers
    "chemfiles/exports.hpp",
    "chemfiles/config.hpp",
    "chemfiles/sorted_set.hpp",
    "chemfiles/unreachable.hpp",
    # chemfiles main headers
    "chemfiles/generic.hpp",
    "chemfiles/types.hpp",
    "chemfiles/Atom.hpp",
    "chemfiles/Frame.hpp",
    "chemfiles/Error.hpp",
    "chemfiles/Residue.hpp",
    "chemfiles/Property.hpp",
    "chemfiles/Topology.hpp",
    "chemfiles/UnitCell.hpp",
    "chemfiles/Trajectory.hpp",
    "chemfiles/Selections.hpp",
    "chemfiles/Connectivity.hpp",
    # chemfiles capi headers
    "chemfiles/capi/atom.h",
    "chemfiles/capi/selection.h",
    "chemfiles/capi/trajectory.h",
    "chemfiles/capi/residue.h",
    "chemfiles/capi/property.h",
    "chemfiles/capi/cell.h",
    "chemfiles/capi/frame.h",
    "chemfiles/capi/types.h",
    "chemfiles/capi/topology.h",
    "chemfiles/capi/misc.h",
]


def error(message):
    global ERRORS
    ERRORS += 1
    print(message)


def included_headers(path):
    includes = set()
    with open(path) as fd:
        for line in fd:
            if "#include" in line:
                matched = re.match("#include\s*[\"<](.*)[\">]", line)
                if not matched:
                    error("bad include in {}: {}".format(path, line))
                header = matched.groups()[0]
                includes.add(header)
                if header.startswith("chemfiles"):
                    if header not in GENERATED_HEADERS:
                        path = os.path.join(ROOT, "include", header)
                        includes.update(included_headers(path))
    return includes


def check_allowded(headers):
    for header in headers:
        if header not in WHITELIST:
            error("private header {} is publicly reachable".format(header))


if __name__ == '__main__':
    headers = included_headers(os.path.join(ROOT, "include", "chemfiles.h"))
    check_allowded(headers)

    headers = included_headers(os.path.join(ROOT, "include", "chemfiles.hpp"))
    check_allowded(headers)

    if ERRORS != 0:
        sys.exit(1)
