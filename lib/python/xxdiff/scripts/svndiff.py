#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xxdiff-svn-diff [<options>] <file> <file> [<file>]

Script that invokes xxdiff for all modified files in the given Subversion
checkout area.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4', 'Subversion']


# stdlib imports.
import sys, os, tempfile
from os.path import *

# xxdiff imports.
import xxdiff.scripts
import xxdiff.invoke
from xxdiff.scripts import tmpprefix
from xxdiff.scm import subversion


#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    parser.add_option('-c', '--commit-after-review', action='store_true',
                      help="Spawns an editor window for adding comments and "
                      "starts the review, then commits the files with the "
                      "given comment.")

    opts, args = parser.parse_args()

    return opts, args


#-------------------------------------------------------------------------------
#
def svndiff_main():
    """
    Main program for svn-diff script.
    """
    opts, args = parse_options()

    # Get the status of the working copy.
    statii = subversion.status(args)

    # First print out the status to the user.
    subversion.print_status(statii)
    print

    msgfmt = '%-16s | %s'
    print msgfmt % ('Action', 'Status')
    print msgfmt % ('-'*16, '-'*40)
    
    # For each of the files reported by status
    for s in statii:
        msg = 'xxdiff'
        dopts = []
        merged = s.filename
        try:
            # Ignore unmodified files if there are any.
            if s.status in (' ', '?'):
                msg = 'ignored'
                continue

            # Diff modified files
            if s.status in ('M', 'C'):
                tmpf = subversion.cat_revision_temp(s.filename, 'BASE')
                left, right = tmpf.name, s.filename

                dopts.extend(['--title1', '%s (BASE)' % s.filename])

            # Diff added files
            elif s.status == 'A':
                # Check if it is a directory.
                if not isfile(s.filename):
                    msg = 'directory, skip'
                    continue

                if s.withhist == '+':
                    # Get the source filename from the history.
                    info = subversion.getinfo(s.filename)
                    from_url, from_rev = [info.get('Copied From %s' % x, None)
                                          for x in 'URL', 'Rev']

                    tmpf = subversion.cat_revision_temp(s.filename, 'BASE')
                    dopts.extend(['--title1', '%s (%s)' % (from_url, from_rev)])
                else:
                    tmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)
                    dopts.extend(['--title1', '(NON-EXISTING)'])

                left, right = tmpf.name, s.filename

            # Diff deleted files
            elif s.status == 'D':
                tmpf = subversion.cat_revision_temp(s.filename, 'BASE')
                tmpf_empty = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

                dopts.extend(['--title1', '%s (BASE)' % s.filename,
                              '--title2', '(DELETED)'])

                left, right = tmpf.name, tmpf_empty.name
                
            # We don't know what to do with the rest yet.
            else:
                msg = 'ignored'
                print >> sys.stderr, (
                    "Error: Action for status '%s' on file '%s' "
                    "is not implemented yet") % (s.status, s.filename)
                continue

        finally:
            print msgfmt % (msg, s.parsed_line)


        # Run xxdiff on the files.
        assert left and right
        if merged is not None:
            dopts.extend(['--merged-filename', merged])
        xxdiff.invoke.xxdiff_display(opts, left, right, *dopts)


#-------------------------------------------------------------------------------
#
def main():
    xxdiff.scripts.interruptible_main(svndiff_main)

if __name__ == '__main__':
    main()

