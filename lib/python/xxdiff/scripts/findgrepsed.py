#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
xxdiff-find-grep-sed [<options>] <regexp> <sed-cmd> [<root> ...]

Perform global sed-like replacements in a set of files.

This program walks a directory hierarchy, selects some files to be processed by
filenames (or by default, process all files found), then each selected file is
searched for some regexp pattern, and if there is a match, run the given file
through a sed command and replace the file with that output.

Notes
-----

- There is an option to request confirmation through xxdiff.

- The script automatically creates backup files.

- The script automatically generates a detailed log of its actions and
  a text summary of all the differences beteween the original and new files.

- The script can optionally checkout the file with ClearCase before performing
  the replacement.

"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.3', 'findutils', 'diffutils', 'sed']


# stdlib imports.
import sys, os, re
from os.path import *
import commands, tempfile, shutil

# xxdiff imports.
import xxdiff.scripts
import xxdiff.selectfiles
import xxdiff.backup
import xxdiff.invoke
import xxdiff.scm
from xxdiff.scripts import tmpprefix


#-------------------------------------------------------------------------------
#
# Diff command template
difffmt = 'diff -y --suppress-common-lines "%s" "%s" 2>&1'


#-------------------------------------------------------------------------------
#
def perform_sed_replace( fn, sedcmd, opts ):
    """
    Process a file through sed.

    Arguments:
    - fn: filename to process -> string
    - sedcmd: the sed command to run -> string
    - opts: program options object -> Options instance
    """

    # Print header
    print '=' * 80
    print
    print 'File:    ', fn
    print 'Absolute:', abspath(fn)
    print

    # Create temporary file
    tmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

    # Sed command string

    # If this is not for show only, make sure right away that we can overwrite
    # the destination file
    if not opts.dry_run and not os.access(fn, os.W_OK):
        raise SystemExit("Error: cannot write to file '%s'." % fn)

    # Perform sed replacement to a temporary file
    replcmd = 'sed -e "%s" "%s" > "%s"' % (sedcmd, fn, tmpf.name)
    status, sed_output = commands.getstatusoutput(replcmd)
    if status != 0:
        raise SystemExit(
            "Error: running sed command:\nFile:%s\n%s" % (fn, sed_output))

    # Run diff between the original and the temp file
    diffcmd = difffmt % (fn, tmpf.name)
    status, diff_output = commands.getstatusoutput(diffcmd)
    if os.WEXITSTATUS(status) == 0:
        print
        print "(Warning: no differences.)"
        print

    # Print output from sed command
    print diff_output
    print

    if opts.dry_run:
        return
    # So this is for real...


    def do_replace( tmpfn, fn ):
        "Replace the original file."
        xxdiff.backup.backup_file(fn, opts, sys.stdout)

        if xxdiff.scm.insure_checkout(fn, opts, sys.stdout):
            print

        # Copy the modified file over the original
        shutil.copyfile(tmpfn, fn)


    if opts.no_confirm:
        # No graphical diff, just replace the files without asking.
        do_replace(tmpf.name, fn)

    else:
        # Call xxdiff on it.
        decision, mergedf = xxdiff.invoke.xxdiff_decision(
            opts, '--title2', 'NEW FILE', fn, tmpf.name)

        print decision
        if decision == 'ACCEPT':
            # Accepted change, perform the replacement
            do_replace(tmpf.name, fn)

        elif decision == 'REJECT' or decision == 'NODECISION':
            # Rejected change (or program killed), do not replace
            pass

        elif decision == 'MERGED':
            # Some user-driven selections saved to the merge file

            # Run diff again to show the real changes that will be applied.
            diffcmd2 = difffmt % (fn, mergedf.name)
            status, diff_output = commands.getstatusoutput(diffcmd2)
            print 'Actual merged changes:'
            print
            print diff_output
            print

            do_replace(mergedf.name, fn)

        else:
            raise SystemExit(
                "Error: unexpected answer from xxdiff: %s" % diff_output)


#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options and return a tuple of

    - regular expression to match files against -> re regexp object
    - sed command -> string
    - root directories to search into -> list of strings
    - options -> Options object
    """

    import optparse
    parser = optparse.OptionParser(__doc__.strip())
    
    xxdiff.backup.options_graft(parser)
    xxdiff.selectfiles.options_graft(parser)
    xxdiff.scm.options_graft(parser)
    xxdiff.invoke.options_graft(parser)

    parser.add_option('-n', '--dry-run', action='store_true',
                      help="Print the commands that would be executed " +
                      "but don't really run them.")

    parser.add_option('-X', '--no-confirm', action='store_true',
                      help="Do not ask for confirmation with graphical "
                      "diff viewer.")

    xxdiff.scripts.install_autocomplete(parser)

    # Parse arguments
    opts, args = parser.parse_args()

    # Check that we got two arguments
    if len(args) < 2:
        parser.error("you must specify at least a regular "
                     "expression and a sed command")

    # Unpack arguments
    regexp, sedcmd = args[:2]
    roots = args[2:] or ['.']  # Root directories, use CWD if none specified

    selector = xxdiff.selectfiles.options_validate(opts, roots)
    xxdiff.backup.options_validate(opts, logs=sys.stdout)
    xxdiff.scm.options_validate(opts)
    xxdiff.invoke.options_validate(opts, logs=sys.stdout)

    # Compile regular expression
    try:
        regexp = re.compile(regexp)
    except re.error:
        parser.error("Cannot compile given regexp '%s'." % regexp)


    return regexp, sedcmd, selector, opts


#-------------------------------------------------------------------------------
#
def findgrepsed_main():
    """
    Main program for find-grep-sed script.
    """
    regexp, sedcmd, selector, opts = parse_options()

    import warnings
    warnings.warn("xxdiff-find-grep-sed is obsolete. "
                  "Use xxdiff-filter with --select-grep option instead.")

    # Perform search and replacement
    for fn in selector:
        try:
            # Grep the file.
            text = open(fn, 'r').read()
            if regexp.search(text, re.MULTILINE):
                # The grep matched, now process the file.
                perform_sed_replace(fn, sedcmd, opts)

        except IOError, e:
            # Don't bail out if we cannot read a file.
            print >> sys.stderr, "Warning: cannot read file '%s'." % fn

    # Print out reminder of location of backup files (for convenience).
    if opts.backup_dir:
        print
        print "Stored backup files under:", opts.backup_dir
        print


#-------------------------------------------------------------------------------
#
def main():
    xxdiff.scripts.interruptible_main(findgrepsed_main)

if __name__ == '__main__':
    main()
    