# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xx-svn-diff [<options>] <file> <file> [<file>]

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
from xxdiff.invoke import title_opts
import xxdiff.editor
import xxdiff.resilient
import xxdiff.utils
from xxdiff.utils import makedirs
import xxdiff.history
from xxdiff.scripts import tmpprefix, script_name
from xxdiff.scm import subversion
from xxdiff.scripts.svnforeign import query_unregistered_svn_files


def review_file(sobj, opts):
    """
    Check the given status object and if necessary, spawn xxdiff on it.

    Return a pair of ((file type description, action) waiter-object).
    """
    msg = ('normal', 'display')
    dopts = []
    merged = sobj.filename
    try:
        # Ignore unmodified files if there are any.
        if sobj.status in (' ', '?'):
            msg = ('unmodified', 'ignored')
            return msg, None

        # Diff modified files
        if sobj.status in ('M', 'C'):
            tmpf = subversion.cat_revision_temp(sobj.filename, 'BASE')
            left, right = tmpf.name, sobj.filename

            dopts.extend(title_opts('%s (BASE)' % sobj.filename))

        # Diff added files
        elif sobj.status == 'A':
            # Check if it is a directory.
            if not isfile(sobj.filename):
                msg = ('directory', 'skip')
                return msg, None

            if sobj.withhist == '+':
                # Get the source filename from the history.
                info = subversion.getinfo(sobj.filename)
                from_url, from_rev = [info.get('Copied From %s' % x, None)
                                      for x in 'URL', 'Rev']

                tmpf = subversion.cat_revision_temp(sobj.filename, 'BASE')
                dopts.extend(title_opts('%s (%s)' % (from_url, from_rev)))
            else:
                tmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)
                dopts.extend(title_opts('(NON-EXISTING)'))

            left, right = tmpf.name, sobj.filename

        # Diff deleted files
        elif sobj.status == 'D':
            tmpf = subversion.cat_revision_temp(sobj.filename, 'BASE')
            tmpf_empty = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

            dopts.extend(title_opts('%s (BASE)' % sobj.filename,
                                    '(DELETED)'))

            left, right = tmpf.name, tmpf_empty.name

        # We don't know what to do with the rest yet.
        else:
            msg = ('unknown', 'ignored')
            print >> sys.stderr, (
                "Error: Action for status '%s' on file '%s' "
                "is not implemented yet") % (sobj.status, sobj.filename)
            return msg, None
    finally:
        pass

    # Check for non-text files.
    if not xxdiff.utils.istextfile(left) or not xxdiff.utils.istextfile(right):
        return ('non-text', 'skip'), None
        
    # Run xxdiff on the files.
    assert left and right
    if merged is not None:
        dopts.extend(['--merged-filename', merged])
    waiter = xxdiff.invoke.xxdiff_display(opts, left, right, nowait=1, *dopts)

    return msg, waiter

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
                      "given comment.  NOTE: you editor is taken from the "
                      "environment variables XXDIFF_EDITOR, SVN_EDITOR, and "
                      "then EDITOR.  This program *MUST* open a new window "
                      "(for example, you cannot use just 'vi' but rather "
                      "something like: xterm -e 'vi %s').")

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

    for mod in xxdiff.invoke, xxdiff.history:
        mod.options_graft(parser)
    xxdiff.backup.options_graft(parser,
                                "These options affect automatic backup of "
                                "deleted files, if enabled.")
    
    xxdiff.scripts.install_autocomplete(parser)

    opts, args = parser.parse_args()

    for mod in xxdiff.backup, xxdiff.invoke, xxdiff.history:
        mod.options_validate(opts, parser)

    if opts.comments_file and not opts.commit:
        print >> sys.stderr, "(Option '%s' ignored.) " % o.dest

    # Add a verbose option for svnforeign to use.
    opts.verbose = True

    return opts, args


def svndiff_main():
    """
    Main program for svn-diff script.
    """
    opts, args = parse_options()

    # Compute the location of the resilient directory for the comments and
    # history files (and maybe more stuff later on).
    resildir = xxdiff.resilient.resilient_for_paths(args)
    hist = xxdiff.history.History(opts, resildir)

    # Compute a list of files to ignore (e.g. if the comments or history file is
    # located in the checkout, we want to ignore them and then associated
    # swap/temp files).
    ignofiles = []
    if opts.commit and opts.comments_file:
        comfn = abspath(opts.comments_file)
        ignofiles.extend([comfn, '%s.swp' % comfn])
    if opts.history and opts.history_file:
        histfn = abspath(opts.history_file)
        ignofiles.extend([histfn, '%s.swp' % histfn])

    if opts.foreign:
        # Consider the unregistered files.
        if query_unregistered_svn_files(
            args, opts, sys.stdout, ignore=ignofiles) is not True:
            # The user has quit, don't continue.
            sys.exit(0)
        print
        print

    # Get the status of the working copy.
    statii = subversion.status(args)

    # Ignore the comments file from the svn status output.
    statii = [s for s in statii if abspath(s.filename) not in ignofiles]
            
    if not statii:
        print '(Nothing to do, exiting.)'
        hist.delete()
        return

    # First print out the status to a string.
    renstatus = os.linesep.join(x.parsed_line for x in statii)

    if opts.commit:
        # File to delete after a succesful commit.
        if opts.comments_file:
            comfn = abspath(opts.comments_file)
        else:
            # Select a comments file and make sure that it exists.
            comfn = join(resildir, 'comments')

            makedirs(resildir, False)
            if not exists(comfn):
                open(comfn, 'w')

    # Spawn an editor if requested before starting the review.
    if opts.commit:
        m = {'date': datetime.datetime.now()}
        comments = None # renstatus
        edit_waiter = xxdiff.editor.spawn_editor(comments, filename=comfn)

    # First print out the status to the user.
    print 'Status Of Files To Be Diffed'
    print '----------------------------'
    print renstatus

    # Then we start printing each file and the associated decision.
    msgfmt = '  %-10s | %-10s | %s'
    print
    print msgfmt % ('Type', 'Action', 'Status')
    print msgfmt % ('-'*10, '-'*10, '-'*40)

    # Main loop for graphical diffs, over each of the files reported by status.
    for s in statii:
        kind, action = 'unknown', 'exception' # Initialize for in case of an
                                              # exception.
        try:
            # Skip directories.
            if isdir(s.filename):
                kind, action = 'directory', 'skip'
                continue
            elif islink(s.filename):
                kind, action = 'symlink', 'skip'
                continue
                
            # Compute unique string for history recorder.  We use the size, last
            # modification time, and status info to hash on this.
            if exists(s.filename):
                fstat = os.lstat(s.filename)
                sz, mtime = fstat.st_size, fstat.st_mtime
            else:
                # Deal with files that have been deleted.
                sz, mtime = 0, 0
            histitem = ' '.join((str(sz), str(mtime), s.parsed_line))

            # If the file has already been reviewed in the history, skip it.
            if hist.check(histitem):
                kind, action = 'seen', 'skip'
                continue

            # Review the file.
            (kind, action), waiter = review_file(s, opts)
        finally:
            print msgfmt % (kind, action, s.parsed_line)

        if waiter is not None:
            waiter()

        # We have succesfully finished viewing the file, add it to the history.
        hist.append(histitem)

    # Commit the files if requested.
    if opts.commit:
        print "\nWaiting for editor '%s' to complete..." % edit_waiter.command,
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

        # Delete temporary comments flie if we created it.
        if not opts.comments_file:
            xxdiff.resilient.resilient_remove(comfn)

    # The entire list of files has been reviewed (and possibly committed), clear
    # the history file.
    if opts.history:
        print 
        print '(Review complete, history cleared)'
        hist.delete()

    
def main():
    xxdiff.scripts.interruptible_main(svndiff_main)

if __name__ == '__main__':
    main()

