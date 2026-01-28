# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import sys
sys.path.insert(0, os.path.abspath('/home/yusuke/var/build/druid/py_capi'))

project = 'Druid'
copyright = '2026, Yusuke Matsunaga'
author = 'Yusuke Matsunaga'
release = '1.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.autosummary',
    'breathe',
]

breathe_projects = {
    "ymworks": "/home/yusuke/var/build/druid/dox-src/xml"
}

breathe_default_project = "ymworks"

source_suffix = {
    '.rst': 'restructuredtext'
}

templates_path = ['_templates']
exclude_patterns = []

language = 'ja'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinxdoc'
html_static_path = ['_static']


def setup(app):
    app.add_css_file('custom.css')
