#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions for Rational ClearCase.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
from subprocess import call 

# xxdiff imports.
from xxdiff.scripts import script_name


#-------------------------------------------------------------------------------
#
def is_checked_out( filename ):
    """
    Check if the file is already checked out.
    """
    return False

    # FIXME: TODO -- you need implement this using 'cleartool describe' the next
    # time that you have access to a ClearCase system.


#-------------------------------------------------------------------------------
#
def checkout( filename ):
    """
    Checkout the given filename from ClearCase.
    """
    msg = 'Checked out by %s' % script_name

    call(['cleartool', 'co', '-comment', msg, fn])
    # FIXME: TODO -- you need to check the return code.


#-------------------------------------------------------------------------------
#
def commit( filename ):
    """
    Commit the given filename into CVS.
    """
    msg = 'Committed by %s' % script_name
    call(['cleartool', 'commit', '-command', msg, filename])


