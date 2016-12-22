#!/usr/bin/env python
# -*- coding: utf8 -*-
"""
A small script checking that all the C API functions are documented, and have
an example.
"""
import os

ROOT = os.path.join(os.path.dirname(__file__), "..", "..")


def documented_functions():
    functions = []
    DOCS = os.path.join(ROOT, "doc", "capi")
    for (root, _, paths) in os.walk(DOCS):
        for path in paths:
            with open(os.path.join(root, path)) as fd:
                for line in fd:
                    if line.startswith(".. doxygenfunction::"):
                        name = line.split()[2]
                        functions.append(name)
    return functions


def function_name(line):
    assert(line.startswith("CHFL_EXPORT"))
    splitted = line.split()
    if splitted[2].startswith("chfl_"):
        name = splitted[2].split('(')[0]
    elif splitted[3].startswith("chfl_"):
        name = splitted[3].split('(')[0]
    else:
        raise RuntimeError(
            "Could not get function name in '" + line + "'"
        )
    return name


def all_functions():
    functions = []
    HEADERS = os.path.join(ROOT, "include", "chemfiles", "capi")
    for (root, _, paths) in os.walk(HEADERS):
        for path in paths:
            with open(os.path.join(root, path)) as fd:
                for line in fd:
                    if line.startswith("CHFL_EXPORT"):
                        functions.append(function_name(line))
    return functions


def check_examples():
    HEADERS = os.path.join(ROOT, "include", "chemfiles", "capi")
    for (root, _, paths) in os.walk(HEADERS):
        for path in paths:
            with open(os.path.join(root, path)) as fd:
                in_doc = False
                example_found = False

                for line in fd:
                    if "@example" in line and in_doc:
                        example_found = True

                    if line.startswith("///"):
                        in_doc = True
                    elif line.startswith("CHFL_EXPORT"):
                        in_doc = False
                        if not example_found:
                            name = function_name(line)
                            raise RuntimeError(
                                "Missing example for {}".format(name)
                            )
                        example_found = False


if __name__ == '__main__':
    docs = documented_functions()
    for function in all_functions():
        if function not in docs:
            raise RuntimeError("Missing documentation for {}".format(function))
    check_examples()
