#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xxdiff-svn [<options>] <file> <file> [<file>]

Script that invokes xxdiff for all modified files in the given Subversion
checkout area.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.3']


#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())
    opts, args = parser.parse_args()

    if len(args) > 3:
        raise parser.error("Cannot invoke wrapper with more than 3 files.")

    return opts, args

#-------------------------------------------------------------------------------
#
def svn_main():
    """
    Main program for cond-replace script.
    """
    opts, args = parse_options()

## FIXME: todo




















#-------------------------------------------------------------------------------
#
main = svn_main

if __name__ == '__main__':
    main()

