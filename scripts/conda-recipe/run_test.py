#!/usr/bin/env python
# -*- coding=utf-8 -*-

import sys
import os

TESTS_PATH = os.path.join(os.environ["SRC_DIR"], "bindings", "python", "tests")
sys.path.append(TESTS_PATH)

import read
import write
