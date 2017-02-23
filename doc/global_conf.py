# -*- coding: utf-8 -*-

import sys
import os
import sphinx_bootstrap_theme
import subprocess

DOC_ROOT = os.path.abspath(os.path.dirname(__file__))
sys.path.append(os.path.join(DOC_ROOT, "ext"))

read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
if read_the_docs_build:
    subprocess.call('cd ..; doxygen', shell=True)

# -- General configuration ------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
# needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.mathjax',
    'sphinx.ext.todo',
    'breathe',
]

# Breathe projects
breathe_projects = {"chemfiles": os.path.join(DOC_ROOT, "doxygen", "xml")}
breathe_default_project = "chemfiles"

# Add any paths that contain templates here, relative to this directory.
templates_path = [os.path.join(DOC_ROOT, "templates")]

# The suffix of source filenames.
source_suffix = '.rst'

# The encoding of source files.
# source_encoding = 'utf-8-sig'

# The master toctree document.
master_doc = 'index'

# General information about the project.
project = u'Chemfiles'


def version():
    with open(os.path.join(DOC_ROOT, "..", "VERSION")) as f:
        full_version = f.read().split('-')
    release = full_version[0]
    version = '.'.join(release.split('.')[0:2])
    if len(full_version) > 1:
        # Developement release
        release += "-dev"
        version += "-dev"
    return (version, release)


version, release = version()

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = []

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# -- Options for HTML output ----------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'bootstrap'
html_theme_path = sphinx_bootstrap_theme.get_html_theme_path()

html_theme_options = {
    'navbar_pagenav_name': "Content",
    'navbar_site_name': "Navigation",
    'source_link_position': None,
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = [os.path.join(DOC_ROOT, "static", "css", "chemfiles.css")]

# Output file base name for HTML help builder.
htmlhelp_basename = 'chemfiles'


# -- Options for LaTeX output ---------------------------------------------

latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    # 'preamble': '',
}

latex_documents = [
    ('index', 'chemfiles.tex', u'Chemfiles Documentation',
     u'Guillaume Fraux', 'howto'),
]
