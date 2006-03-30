#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to invoke xxdiff in various ways.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import sys, os, shutil, optparse, tempfile
from os.path import *

# xxdiff imports.
from xxdiff.scripts import tmpprefix


#-------------------------------------------------------------------------------
#
def options_graft( parser ):
    """
    Graft options on given parser for invocations of xxdiff.
    """
    group = optparse.OptionGroup(parser, "Options for xxdiff subprocesses",
                                 "These options affect how xxdiff is called.")

    group.add_option('-X', '--xxdiff-exec', action='store', metavar='XXDIFF',
                     default='xxdiff',
                     help="Specifies which xxdiff executable to use (default "
                     "is looked up in PATH.")

    group.add_option('-O', '--xxdiff-options', '--xxdiff-opts',
                     action='store', metavar="OPTS",
                     default='',
                     help="Additional options to pass on to xxdiff.")

    parser.add_option_group(group)

    return group


def options_validate( opts, logs=None ):
    """
    Validate xxdiff options.
    """
    pass


#-------------------------------------------------------------------------------
#
def confirm( opts, *arguments ):
    """
    Runs xxdiff with the given arguments, passed directly to subprocess.call().
    """
    FIXME TODO



#-------------------------------------------------------------------------------
#
def test():
    """
    Test stub.
    """


        
if __name__ == '__main__':
    test()

