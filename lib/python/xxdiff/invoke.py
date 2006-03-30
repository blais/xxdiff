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
def confirm( opts, create_merged_temp=False, *arguments ):
    """
    Runs xxdiff with the given arguments, passed directly to subprocess.call().
    Return the decision code, if there was one requested.

    If 'create_merged_temp' is specified, we create a temporary file to be used
    as the merged output to be collected and modify the options accordingly.
    """
    raise NotImplementedError
    ## FIXME TODO

    # Use opts.xxdiff_exec and opts.xxdiff_options



#-------------------------------------------------------------------------------
#
def test():
    """
    Test stub.
    """



if __name__ == '__main__':
    test()

