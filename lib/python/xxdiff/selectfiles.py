#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to select files within directories.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, re, optparse


#-------------------------------------------------------------------------------
#
def select_patterns( roots, select, ignore ):
    """
    Generator that selects files to process by regexps.

    Arguments:
    - roots: root directories -> list of strings
    - select: patterns to select -> list of re regexp objects
    - ignore: patterns to ignore -> list of re regexp objects

    Yields: selected filenames -> string
    """
    # Walk the tree of files and select files as requested
    for root in roots:
        for dn, dirs, files in os.walk(root):
            for d in list(dirs):
                add = False
                for sre in select:
                    if sre.match(d):
                        add = True
                        break
                for ire in ignore:
                    if ire.match(d):
                        add = False
                        break
                if not add:
                    dirs.remove(d)

            for fn in files:
                add = False
                for sre in select:
                    if sre.match(fn):
                        add = True
                        break
                for ire in ignore:
                    if ire.match(fn):
                        add = False
                        break
                if add:
                    yield join(dn, fn)

#-------------------------------------------------------------------------------
#
def select_from_file( fn ):
    """
    Generator that yields selected filenames read from a file.

    Arguments:
    - fn: filename of the file that contains the list of filenames -> string

    Yields: selected filenames -> string
    """
    try:
        f = open(fn, 'r')
        for fn in f.xreadline():
            yield fn.strip()
    except IOError, e:
        raise SystemExit("Error: cannot open/read file (%s)" % e)


#-------------------------------------------------------------------------------
#
def add_select_optgroup( parser ):
    """
    Adds a new option group for file selection options to the given parser and
    returns this new group.
    """

    group = optparse.OptionGroup(parser, "File selection options",
                                 "These options affect which files are "
                                 "selected for grepping in the first place.")

    group.add_option('-s', '--select', action='append',
                     metavar="REGEXP", default=[],
                     help="Adds a regular expression for filenames to "
                     "process.")

    group.add_option('-I', '--ignore', action='append',
                     metavar="REGEXP", default=[],
                     help="Adds a regular expression for files to ignore.")

    # Note: we could improve this as soon as Python will include a more recent
    # version of Optik, which implements the action 'append_const'.
    def select_cpp_cb(option, opt, value, parser):
        parser.values.select.append('.*\.(h(pp)?|c(pp|\+\+|c)?)')
    group.add_option('-c', '--select-cpp',
                     action='callback', callback=select_cpp_cb,
                     help="Adds a regular expression for selecting C++ "
                     "files to match against.")

    group.add_option('-f', '--select-from-file', action='store',
                     metavar='FILE',
                     help="Do not run find but instead use the list of files "
                     "in the specified filename.")

    parser.add_option_group(group)

    return group



#-------------------------------------------------------------------------------
#
def test():
    """
    Test stub for selection options.
    """
    parser = optparse.OptionParser(__doc__.strip())
    add_select_optgroup(parser)
    opts, args = parser.parse_args()

    for optname, optvalue in opts.__dict__.iteritems():
        print optname, '=', optvalue

        
if __name__ == '__main__':
    test()

