#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Common code for all the scripts.

This directory contains the actual scripts that are run from the stubs.  Any
significant code should be shared in the parent directory.  These files should
contain nothing but the high-level algorithms and the options parsing for the
scripts.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


#-------------------------------------------------------------------------------
#
def interruptible_main( mainfun ):
    """
    Wrapper to allow keyboard interrupt signals.
    """
    try:
        mainfun()
    except KeyboardInterrupt:
        # If interrupted, exit nicely
        raise SystemExit('Interrupted.')


#-------------------------------------------------------------------------------
#
def install_autocomplete( parser ):
    """
    Install automatic programmable completion support if available.
    Scripts should work even if it's not there.
    """
    try:
        import optcomplete
        optcomplete.autocomplete(parser)
    except ImportError:
        pass

