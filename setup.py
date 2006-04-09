#!/usr/bin/env python
#
# Install script for tengis.
#

__version__ = "$Revision$"
__author__ = "Martin Blais <blais@furius.ca>"

import os
from os.path import join
from distutils.core import setup

def read_version():
    try:
        return open('VERSION', 'r').readline().strip()
    except IOError, e:
        raise SystemExit(
            "Error: you must run setup from the root directory (%s)" % str(e))

allscripts = [join('bin', x) for x in os.listdir('bin')
              if x.startswith('xxdiff-')]
allscripts.append(join('bin', 'svn-foreign'))

setup(name="xxdiff-scripts",
      version=read_version(),
      description=\
      "Python package for writing scripts around xxdiff",
      long_description="""
This package provides a number of scripts that are used to perform a variety of
tasks that all involve getting user verification and feedback using the xxdiff
graphical differences viewer.

For example, there are scripts to perform global renaming of strings within a
large codebase, where each transformed file is viewed by the user with an
xxdiff, accepted, rejected or merged changes written over the original file
(file backups are supported).  Also, this infrastructure is mostly provided as
modules, in order to allow users to write file transformations in Python and to
leverage this interactive confirmation process.

There are also scripts that visualize diffs for a number of SCM systems, like
CVS, Subversion, etc.  This package was born after a number of these useful
scripts had sprouted, and it became apparent that sharing the common code for
the scripts would be a great advantage to tools writers.

See documentation for a full list of the scripts and their role:
http://furius.ca/xxdiff/doc/xxdiff-scripts.html
""",
      license="GNU GPL",
      author="Martin Blais",
      author_email="blais@furius.ca",
      url="http://furius.ca/xxdiff",
      package_dir = {'': 'lib/python'},
      packages = ['xxdiff',
                  join('xxdiff', 'scripts'),
                  join('xxdiff', 'scm')],
      scripts = allscripts
     )

