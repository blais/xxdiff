# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions for CVS.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, re
from subprocess import call


def is_checked_out(filename):
    """
    Return True if the file is checked out.
    """
    return os.access(os.W_OK)


def checkout(filename):
    """
    Checkout the given filename from CVS.
    """
    call(['cvs', 'edit', filename])


def commit(filename):
    """
    Commit the given filename into CVS.
    """
    call(['cvs', 'commit', filename])


# Note: this is cut-n-paste from cvs.py, for simplicity of distribution.
def unmerge2(text):
    """
    Unmerges conflicts between two files and returns the two split original
    files.  This is not meant to work with three files.
    """
    begre = re.compile('^<<<<<<< (.*)')
    sepre = re.compile('^=======\s*$')
    endre = re.compile('^>>>>>>> (.*)')

    OUT, IN1, IN2 = 0, 1, 2

    status = OUT
    lines1, lines2 = [], []
    for l in text.splitlines():
        if status == OUT:
            if begre.match(l):
                status = IN1
            else:
                lines1.append(l)
                lines2.append(l)
        elif status == IN1:
            if sepre.match(l):
                status = IN2
            else:
                lines1.append(l)
        elif status == IN2:
            if endre.match(l):
                status = OUT
            else:
                lines2.append(l)

    text1 = os.linesep.join(lines1)
    text2 = os.linesep.join(lines2)

    return text1, text2

