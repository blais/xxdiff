#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xxdiff-svn-diff [<options>] <file> <file> [<file>]

Script that invokes xxdiff for all modified files in the given Subversion
checkout area.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4', 'Subversion']


# stdlib imports.
import sys, os, tempfile, datetime
from os.path import *

# xxdiff imports.
import xxdiff.scripts
import xxdiff.invoke
import xxdiff.editor
from xxdiff.scripts import tmpprefix
from xxdiff.scm import subversion
from xxdiff.scripts.svnforeign import query_unregistered_svn_files


#-------------------------------------------------------------------------------
#
def review_file( sobj, opts ):
    """
    Check the given status object and if necessary, spawn xxdiff on it.
    """
    msg = 'xxdiff'
    dopts = []
    merged = sobj.filename
    try:
        # Ignore unmodified files if there are any.
        if sobj.status in (' ', '?'):
            msg = 'ignored'
            return msg, None

        # Diff modified files
        if sobj.status in ('M', 'C'):
            tmpf = subversion.cat_revision_temp(sobj.filename, 'BASE')
            left, right = tmpf.name, sobj.filename

            dopts.extend(['--title1', '%s (BASE)' % sobj.filename])

        # Diff added files
        elif sobj.status == 'A':
            # Check if it is a directory.
            if not isfile(sobj.filename):
                msg = 'directory, skip'
                return msg, None

            if sobj.withhist == '+':
                # Get the source filename from the history.
                info = subversion.getinfo(sobj.filename)
                from_url, from_rev = [info.get('Copied From %s' % x, None)
                                      for x in 'URL', 'Rev']

                tmpf = subversion.cat_revision_temp(sobj.filename, 'BASE')
                dopts.extend(['--title1', '%s (%s)' % (from_url, from_rev)])
            else:
                tmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)
                dopts.extend(['--title1', '(NON-EXISTING)'])

            left, right = tmpf.name, sobj.filename

        # Diff deleted files
        elif sobj.status == 'D':
            tmpf = subversion.cat_revision_temp(sobj.filename, 'BASE')
            tmpf_empty = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

            dopts.extend(['--title1', '%s (BASE)' % sobj.filename,
                          '--title2', '(DELETED)'])

            left, right = tmpf.name, tmpf_empty.name

        # We don't know what to do with the rest yet.
        else:
            msg = 'ignored'
            print >> sys.stderr, (
                "Error: Action for status '%s' on file '%s' "
                "is not implemented yet") % (sobj.status, sobj.filename)
            return msg, None
    finally:
        pass

    # Run xxdiff on the files.
    assert left and right
    if merged is not None:
        dopts.extend(['--merged-filename', merged])
    waiter = xxdiff.invoke.xxdiff_display(opts, left, right, nowait=1, *dopts)

    return msg, waiter

#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    parser.add_option('-c', '--commit', '--commit-with-comments',
                      action='store_true',
                      help="Spawns an editor window for adding comments and "
                      "starts the review, then commits the files with the "
                      "given comment.")

    o = parser.add_option('-C', '--comments-file', action='store',
                          default=None,
                          help="Specified the comments file to use.  This file "
                          "is left behind and its contents reused if it is "
                          "specified.")

    parser.add_option('-f', '--foreign', '--consider-foreign-files',
                      action='store_true',
                      help="Before starting the review/diffs, check all the "
                      "unregistered files and ask the user one by one about "
                      "what to do with them.")

    xxdiff.backup.options_graft(parser,
                                "These options affect automatic backup of "
                                "deleted files, if enabled.")
    
    opts, args = parser.parse_args()

    xxdiff.backup.options_validate(opts, parser)

    if opts.comments_file and not opts.commit:
        print >> sys.stderr, "(Option '%s' ignored.) " % o.dest

    return opts, args


#-------------------------------------------------------------------------------
#
def svndiff_main():
    """
    Main program for svn-diff script.
    """
    opts, args = parse_options()

    if opts.foreign:
        # Consider the unregistered files.
        if query_unregistered_svn_files(args, opts, sys.stdout) != True:
            # The user has quit, don't continue.
            sys.exit(0)
        print
        print

    # Get the status of the working copy.
    statii = subversion.status(args)

    if not statii:
        print '(Nothing to do, exiting.)'
        return 0

    # First print out the status to a string.
    renstatus = os.linesep.join(x.parsed_line for x in statii)

    # Spawn an editor if requested before starting the review.
    if opts.commit:
        m = {'date': datetime.datetime.now()}
        comments = renstatus
        edit_waiter = xxdiff.editor.spawn_editor(comments,
                                                 filename=opts.comments_file)

    # Get the status of the working copy.
    statii = subversion.status(args)

    # First print out the status to the user.
    print 'Status Of Files To Be Diffed'
    print '----------------------------'
    print renstatus

    # Then we start printing each file and the associated decision.
    msgfmt = '  %-16s | %s'
    print
    print msgfmt % ('Action', 'Status')
    print msgfmt % ('-'*16, '-'*40)

    # Main loop for graphical diffs, over each of the files reported by status.
    for s in statii:
        msg, waiter = review_file(s, opts)
        print msgfmt % (msg, s.parsed_line)
        if waiter is not None:
            waiter()

    # Commit the files if requested.
    if opts.commit:
        print '\nWaiting for editor to complete...',
        sys.stdout.flush()
        comments = edit_waiter()
        print 'Done.\n'
        print 'Recorded Merge Comments: ',
        if comments == '':
            print '(None)'
            comments = None
        else:
            print
            print '-' * 70
            print comments
            print '-' * 70

        subversion.commit(args, comments=comments)


#-------------------------------------------------------------------------------
#
def main():
    xxdiff.scripts.interruptible_main(svndiff_main)

if __name__ == '__main__':
    main()

