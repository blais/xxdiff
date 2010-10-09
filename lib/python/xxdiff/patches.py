# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to manipulate patches.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import re


def splitpatch(text):
    """
    Split output in chunks starting with ^Index.  Returns a list of pairs
    (tuples), each with (filename, patch) contents.
    """

    ## splitre = re.compile('^Index: (.*)$', re.M)
    splitre = re.compile('^Index: (.*)$', re.M)
    chunks = []
    curbeg, curfn = None, None
    for mo in splitre.finditer(text):
        if curbeg != None:
            assert curfn
            chunks.append( (curfn, text[curbeg:mo.start()]) )
        curbeg = mo.start()
        curfn = mo.group(1)
    if curbeg != None:
        chunks.append( (curfn, text[curbeg:]) )

    return chunks


