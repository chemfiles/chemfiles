# -*- coding: utf-8 -*-

import sys
import os

sys.path.append("@CMAKE_CURRENT_SOURCE_DIR@")
from conf import *

DOC_ROOT = os.path.abspath(os.path.dirname(__file__))
breathe_projects = {"Chemharp": os.path.join(DOC_ROOT, "doxygen", "xml")}
