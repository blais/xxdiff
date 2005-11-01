#!/usr/bin/env python
#******************************************************************************\
#* $Source$
#* $Id: xxdiff-subversion 854 2005-10-17 14:11:03Z blais $
#* $Date: 2005-10-17 10:11:03 -0400 (Mon, 17 Oct 2005) $
#*
#* Copyright (C) 2004 Martin Blais <blais@furius.ca>
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

"""xxdiff-subversion [<options>] <file> <file> [<file>]

Simple wrapper script for integration with Subversion in user's configuration
file.  This is meant to be used in a user's configuration file to set diff-cmd
and diff3-cmd.

Add the following lines ``~/.subversion/config``::

  diff-cmd = xxdiff-subversion
  diff3-cmd = xxdiff-subversion

"""

__version__ = "$Revision: 854 $"
__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.3']
__copyright__ = """Copyright (C) 2003-2004 Martin Blais <blais@furius.ca>.
This code is distributed under the terms of the GNU General Public License."""

#===============================================================================
# EXTERNAL DECLARATIONS
#===============================================================================

import sys, os
import commands, shutil
from tempfile import NamedTemporaryFile

#===============================================================================
# LOCAL DECLARATIONS
#===============================================================================

tmppfx = '%s.' % os.path.basename(sys.argv[0])

#===============================================================================
# MAIN
#===============================================================================

def main():
    import optparse
    parser = optparse.OptionParser(__doc__.strip(), version=__version__)

    # For diff-cmd invocation.
    parser.add_option('-u', action='store_true',
                      help="See diff(1). Ignored for xxdiff.")
    parser.add_option('-L', '--label', action='append', default=[],
                      help="See diff(1). Label gets set as file title string "
                      "for xxdiff.")

    # For diff3-cmd invocation.
    parser.add_option('-E', '--show-overlap', action='store_true',
                      help="See diff3(1). Ignored for xxdiff.")
    parser.add_option('-m', '--merge', action='store_true',
                      help="See diff3(1). Passed to xxdiff for automatic "
                      "selection of non-conflictual regions..")

    ## parser.add_option('--reverse', action='store_true',
    ##                   help="Reverse the display order of the files.")
    opts, args = parser.parse_args()

    if len(args) > 3:
        raise parser.error("Cannot invoke wrapper with more than 3 files.")

    # create temporary file to hold merged results.
    tmpf = NamedTemporaryFile('rw', prefix=tmppfx)

    # run command
    xxdiff_path = 'xxdiff'

    decision = False
    cmd = [xxdiff_path]
    if decision:
        cmd.append('--decision')
    if opts.merge:
        cmd.append('--merge')
    cmd.append('--merged-filename="%s"' % tmpf.name)
    for o1, o2, fn in zip(['--title1', '--title2', '--title3'],
                          ['"%s"' % x for x in opts.label],
                          args):
        if os.path.basename(fn) == 'empty-file':
            o2 = '"(NON-EXISTANT FILE)"'
        cmd.extend([o1, o2])
    cmd.extend(args)
    ##print ' '.join(cmd)

    s, o = commands.getstatusoutput(' '.join(cmd))

    if decision:
        # if the user merged, copy the merged file over the original.
        if o in ['MERGED', 'ACCEPT', 'REJECT']:
            tmpf.flush()
            tmpf.seek(0)
            text = tmpf.read()
            tmpf.close()
            sys.stdout.write(text)
    
        elif o == 'NODECISION':
            pass # do nothing
        else:
            raise SystemExit(
                    "Error: unexpected answer from xxdiff: %s" % o)

if __name__ == '__main__':
    main()
