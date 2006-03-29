#!/usr/bin/env python
#******************************************************************************\
#* Copyright (C) 2003-2006 Martin Blais <blais@furius.ca>
#*
#* This program is free software; you can redistribute it and/or modify
#* it under the terms of the GNU General Public License as published by
#* the Free Software Foundation; either version 2 of the License, or
#* (at your option) any later version.
#*
#* This program is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#*
#* You should have received a copy of the GNU General Public License
#* along with this program; if not, write to the Free Software
#* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#*
#*****************************************************************************/

"""xxdiff-match [<options>] <file> <file> <file> <file> ...

Wrapper script for xxdiff that implements an invocation syntax like this:

  xxdiff-match dir1/*.c dir2/*.c

The way we implement the heuristic to match up files is simple: we match up the
basenames of the files.  If there are exactly two or three files in the list, we
invoke an xxdiff on these files.  The files are always invoked in the order that
they show up on the command-line.

Another way to achieving similar behaviour is to use a more generic file
matching script (see 'match-files' by the same author) along with xargs, e.g.::

  match-files dir1/*.c dir2/*.c | xargs -n2 xxdiff

Using xxdiff-match has the advantage that it work with matches of 2 or 3 files,
and also spawns xxdiff on single files (with an empty side).

Note: many users do not seem to understand why such a script is necessary. One
must realize a few things: 1) globbing pattern expansion is done by the shell,
before invocation of xxdiff, thus xxdiff cannot be aware of the globbing
patterns (unless it itself would support the heuristic mentioned here, along
with the user quoting the globbing patterns to avoid shell expansion); 2) the
solution to this problem using expanded file lists is not trivial, the input
(and its ordering) can encompass a lot more possibilities than immediately
appears. Nevertheless, we built this functionality/heuristic in this script,
because it may be useful to some.

See 'match-files' script by the same author for a more generic implementation of
this file matching heuristic.
"""

__version__ = "$Revision: 766 $"
__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.3']
__copyright__ = """Copyright (C) 2003-2004 Martin Blais <blais@furius.ca>.
This code is distributed under the terms of the GNU General Public License."""

# stdlib imports.
import sys, os


#-------------------------------------------------------------------------------
#
def passthruopts( argv ):
    """
    Hack to allow passing some options.  This hack may fail when the given
    options are incorrect but has the advantage that it doesn't require
    knowledge of a specific xxdiff command-line (will not rot as fast).
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


#-------------------------------------------------------------------------------
#
def main():
    try:
        # get options
        opts, files = passthruopts(sys.argv)

        # build map of basenames
        bnmap = {}
        for fn in files:
            dn, bn = os.path.split(fn)
            bnmap.setdefault(bn, []).append(fn)

        # invoke xxdiff's on alphabetical order of the basenames
        bnkeys = bnmap.keys()
        bnkeys.sort()
        for bn in bnkeys:
            l = bnmap[bn]
            if len(l) == 2 or len(l) == 3:
                extra = []
            elif len(l) == 1:
                extra = ['--single']
            else:
                # ignore the files.
                continue

            print ' '.join(l)
            cmd = ' '.join( ['xxdiff'] + extra + opts + l )
            os.popen(cmd)

    except KeyboardInterrupt, e:
        print >> sys.stderr, 'Interrupted.'
        sys.exit(1)

if __name__ == '__main__':
    main()

