# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Misc utilities.  No, really, they are really miscallenous, and unrelated.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, re
from os.path import exists
from curses.ascii import isalnum
from subprocess import Popen, PIPE


def consepairs(seq):
    """
    Iterate over consecutive pairs.
    """
    it = iter(seq)
    prev = it.next()
    for el in it:
        yield prev, el
        prev = el


def idify(s, strip=True, preserve_chars=[]):
    """
    Removes non-alphanumeric characters from a string, morphing them into
    underscores.
    """
    ss = ''
    preserve_list = ['\n'] + preserve_chars
    for c in s:
        if isalnum(c):
            ss += c
        elif c in preserve_list:
            ss += c
        else:
            ss += '_'
    if strip:
        b, e = 0, len(ss)
        while b < len(ss) and ss[b] == '_': b += 1
        while e > 1 and ss[e-1] == '_': e -= 1
        ss = ss[b:e]
    return ss


# Note: there has been a 'file' command for a long time under UNIX.  We favor
# the short options to promote portability.  The --bried and --dereference
# options were taken from Ian F. Darwin's file implementation.
guesscmd = ['file', '-b', '-L']
text_re = re.compile('\\btext\\b')
empty_re = re.compile('^empty$')

def istextfile(fn):
    """
    Attempts to guess if the file indicated by the given filename is a text file
    or a binary file.
    """

    # Unfortunately 'file' does not return an appropriate return code when there
    # is an error.  On top of that, it returns its errors on stdout.  We need to
    # parse the output.
    p = Popen(guesscmd + [fn], stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    if p.returncode != 0 or stderr or stdout.startswith('cannot open'):
        raise RuntimeError("Error: running 'file' on '%s'." % fn)

    return bool(text_re.search(stdout) or empty_re.match(stdout))


def makedirs(dirn, error_on_exist=True):
    """
    A replacement for os.makedirs() that has an option to avoid complaining if
    the destination directory already exists.
    """
    if error_on_exist is False:
        if exists(dirn):
            return 
    os.makedirs(dirn)

