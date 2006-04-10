#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Misc utilities.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
from curses.ascii import isalnum


#-------------------------------------------------------------------------------
#
def consepairs( seq ):
    """
    Iterate over consecutive pairs.
    """
    it = iter(seq)
    prev = it.next()
    for el in it:
        yield prev, el
        prev = el


#-------------------------------------------------------------------------------
#
def idify( s, strip=True, preserve_chars=[] ):
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

