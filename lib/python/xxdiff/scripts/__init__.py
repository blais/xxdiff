# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Common command-line/script -related code for all the scripts.

This directory contains the actual scripts that are run from the stubs.  Any
significant code should be shared in the parent directory.  These files should
contain nothing but the high-level algorithms and the options parsing for the
scripts.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import sys
from os.path import basename


# Check interpreter version requirements.
if sys.version_info[:2] < (2, 4):
    raise SystemExit(
        "Error: you need at least version 2.4 of Python or greater.")


def interruptible_main(mainfun):
    """
    Wrapper to allow keyboard interrupt signals.
    """
    try:
        mainfun()
    except KeyboardInterrupt:
        # If interrupted, exit nicely
        raise SystemExit('Interrupted.')


def install_autocomplete(parser):
    """
    Install automatic programmable completion support if available.
    Scripts should work even if it's not there.
    """
    try:
        import optcomplete
        optcomplete.autocomplete(parser)
    except ImportError:
        pass


def passthruopts(argv):
    """
    Hack to allow passing some options.  This hack may fail when the given
    options are incorrect but has the advantage that it doesn't require
    knowledge of a specific xxdiff command-line (it will not rot as fast).
    """
    try:
        i = argv.index('--endopts')
        opts, args = argv[1:i], argv[i+1:]
    except ValueError:
        for i in xrange(len(argv)-1, -1, -1):
            if argv[i].startswith('-'):
                break
        opts, args = argv[1:i+1], argv[i+1:]
    return opts, args


# Default prefix for temporary files uses the invoking script's name.
script_name = basename(sys.argv[0])
tmpprefix = '%s.' % script_name

