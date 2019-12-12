#!/usr/bin/env python
# -*- coding: utf8 -*-
"""
Check that all error messages are formatted the same way.
"""
from __future__ import print_function
import os
import sys
import glob
import re
import codecs

ROOT = os.path.join(os.path.dirname(__file__), "..", "..")
ERRORS = 0

# Path, line of messages that do not follow the current format, but
# are allowed
ALLOWED = [
    "found deprecated configuration file at '{}', please rename it to .chemfiles.toml",
]


def error(message):
    global ERRORS
    ERRORS += 1
    print(message)


def extract_messages(lines, i, has_context):
    if has_context:
        context_start = lines[i].find('"')
        context_stop = lines[i].find('"', context_start + 1)
    else:
        context_stop = -1

    start = lines[i].find('"', context_stop + 1)
    if start == -1:
        # look in the next line
        i = i + 1
        start = lines[i].find('"')
        if start == -1:
            print("warning: could not get the message at {}:{}".format(
                path, i
            ))
            return ""

    stop = lines[i].find('"', start + 1)
    if stop == -1:
        error("could not get the message end at {}:{}".format(
            path, i
        ))

    return lines[i][start + 1:stop]


def check_message(path, line, message):
    if not message:
        return

    if message in ALLOWED:
        return

    # check for upper case at the start, allow abbreviations (LAMMPS, HELIX)
    if message[0].isupper() and not message[1].isupper():
        error(
            "message starts in upper case\n"
            "    => '{}' at {}:{}".format(message, path, line)
        )

    # check for full stops in sentences
    if re.search("[a-zA-Z][\\.!?]", message):
        error(
            "found punctuation (., ! or ?) in message\n"
            "    => '{}' at {}:{}".format(message, path, line)
        )


def check_file(path):
    with codecs.open(path, encoding="utf8") as fd:
        lines = [l for l in fd]

    for (i, line) in enumerate(lines):
        # ignore comments
        if line.strip().startswith("//"):
            continue

        if " warning(" in line:
            check_message(path, i, extract_messages(lines, i, has_context=True))

        if "throw" in line:
            check_message(path, i, extract_messages(lines, i, has_context=False))


if __name__ == '__main__':
    for path in glob.glob(os.path.join(ROOT, "src/*.cpp")):
        check_file(path)

    for path in glob.glob(os.path.join(ROOT, "src/*/*.cpp")):
        check_file(path)

    for path in glob.glob(os.path.join(ROOT, "include/chemfiles/*.hpp")):
        check_file(path)

    for path in glob.glob(os.path.join(ROOT, "include/chemfiles/*/*.hpp")):
        if "external" in path:
            continue

        check_file(path)

    if ERRORS != 0:
        sys.exit(1)
