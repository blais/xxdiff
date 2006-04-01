#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions for Subversion.
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
    There is no real global mutual editing policy in Subversion.  The restricted
    locking mechanism that is available is not meant to be automated (on the
    contrary).
    """
    return True


#-------------------------------------------------------------------------------
#
def checkout( filename ):
    """
    Checkout the given filename from Subversion.
    """
    return 


#-------------------------------------------------------------------------------
#
def commit( filename ):
    """
    Commit the given filename into CVS.
    """
    msg = 'Committed by %s' % script_name
    call(['svn', 'commit', '-m', msg, filename])


