# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions for Perforce.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import re
from subprocess import Popen, call, PIPE

# xxdiff imports.
from xxdiff.scripts import script_name


P4EXEC = 'g4' # Temporary.


def is_checked_out(filename):
    """
    Check if the file is already checked out.
    """
    p = Popen([P4EXEC, 'desc', '-n', filename], stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    if p.returncode != 0:
        raise SystemExit( stderr )

    if re.search('opened for edit', stdout.strip()):
        return True

    return False

def checkout(filename):
    """
    Checkout the given filename for writable edit in Perforce.
    """
    call([P4EXEC, 'edit', filename])


def commit(filename):
    """
    Commit the given filename into a CL.
    """
    call([P4EXEC, 'change', filename])
