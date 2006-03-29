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



#-------------------------------------------------------------------------------
#

# Prefix for temporaray files
tmpprefix = '%s.' % basename(sys.argv[0])

# Diff command template
difffmt = 'diff -y --suppress-common-lines "%s" "%s" 2>&1'

#-------------------------------------------------------------------------------
#
def backup( fn, opts ):
    """
    Compute backup filename and copy backup file.

    Arguments:
    - fn: filename to backup -> string
    - opts: program options -> Options instance
    """

    # Compute destination backup filename
    if opts.backup_type == 'parallel':
        # Search for a non-existing backup filename alongside the original
        # filename
        fmt = '%s.bak.%%d' % fn
        ii = 1
        while 1:
            backupfn = fmt % ii
            if not exists(backupfn):
                break
            ii += 1

    elif opts.backup_type == 'other':
        # Backup to a different directory
        fn = normpath(fn)
        if isabs(fn):
            fn = fn[1:]
        backupfn = join(opts.backup_dir, normpath(fn))

    else:
        backupfn = None

    if backupfn:
        # Perform the backup
        print 'Backup:', backupfn

        # Make sure that the destination directory exists
        ddn = dirname(backupfn)
        if ddn and not exists(ddn):
            os.makedirs(ddn)

        # Copy the original to the backup directory
        shutil.copy2(fn, backupfn)


#-------------------------------------------------------------------------------
#
def overwrite_original( fromfn, tofn, checkout ):
    """
    Copy a file to a destination, checking out the destination file from
    Clearcase if requested.

    Arguments:
    - fromfn: source file
    - tofn: destination file
    - opts
    """
    if checkout:
        print 'Checking out the file'
        os.system('cleartool co -nc "%s"' % tofn)
        print
        # FIXME: todo, please check the return status
        # Note: output from system() automatically goes to stdout

    shutil.copyfile(fromfn, tofn)

#-------------------------------------------------------------------------------
#
def replace( fn, sedcmd, opts ):
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

        # Backup the file
        backup(fn, opts)

        # Copy the modified file over the original
        overwrite_original(tmpfn, fn, opts.checkout_clearcase)


    if opts.no_confirm:
        # No graphical diff, just replace the files without asking.
        do_replace(tmpf.name, fn)

    else:
        # Create a temporary file to contain the merged results.
        tmpf2 = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

        # Call xxdiff on it.
        status, xxdiff_output = commands.getstatusoutput(
            ('xxdiff --decision --merged-filename "%s" '
             '--title2 "NEW FILE" "%s" "%s" ') %
            (tmpf2.name, fn, tmpf.name))

        print xxdiff_output

        # Check out the exit code from xxdiff
        l1 = xxdiff_output.splitlines()[0]   # first line

        if l1 == 'ACCEPT':
            # Accepted change, perform the replacement
            do_replace(tmpf.name, fn)

        elif l1 == 'REJECT' or l1 == 'NODECISION':
            # Rejected change (or program killed), do not replace
            pass

        elif l1 == 'MERGED':
            # Some user-driven selections saved to the merge file

            # Run diff again to show the real changes that will be applied.
            diffcmd2 = difffmt % (fn, tmpf2.name)
            status, diff_output = commands.getstatusoutput(diffcmd2)
            print 'Actual merged changes:'
            print
            print diff_output
            print

            do_replace(tmpf2.name, fn)

        else:
            raise SystemExit(
                "Error: unexpected answer from xxdiff: %s" % diff_output)


#-------------------------------------------------------------------------------
#
def check_replace( fn, regexp, sedcmd, opts ):
    """
    Grep a file and perform the replacement if necessary.

    Arguments:
    - fn: filename -> string
    - regexp: regular expression object to check contents with -> string
    - sedcmd: sed command to run if match -> string
    - opts: program options -> Options instance
    """
    processed = False
    try:
        text = open(fn, 'r').read()
        if regexp.search(text, re.MULTILINE):
            replace(fn, sedcmd, opts)
            processed = True
    except IOError, e:
        # Don't bail out if we cannot read a file.
        print >> sys.stderr, "Warning: cannot read file '%s'." % fn

    return processed

















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

    parser.add_option('-b', '--backup-type', action='store',
                      type='choice',
                      metavar="CHOICE",
                      choices=['parallel', 'other', 'none'],
                      default='other',
                      help="Selects the backup type "
                      "('parallel', 'other', 'none')")

    parser.add_option('--backup-dir', action='store',
                      help="Specify backup directory for type 'other'")

    parser.add_option('-C', '--checkout-clearcase', action='store_true',
                      help="Checkout files with clearcase before storing.")

    parser.add_option('-n', '--dry-run', action='store_true',
                      help="Print the commands that would be executed " +
                      "but don't really run them.")

    parser.add_option('-X', '--no-confirm', action='store_true',
                      help="Do not ask for confirmation with graphical "
                      "diff viewer.")

    xxdiff.selectfiles.add_select_optgroup(parser)

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

    # Validate backup options
    if opts.backup_type == 'other':
        if not opts.backup_dir:
            # Create backup directory.
            #
            # Note: mkdtemp() leaves with us the responsibility to delete the
            # temporary directory or not.
            opts.backup_dir = tempfile.mkdtemp(prefix=tmpprefix)
        print "Storing backup files under:", opts.backup_dir
        print
    else:
        # Parallel backups.
        if opts.backup_dir:
            parser.error(
                "backup-dir is only valid for backups of type 'other'.")





## FIXME move to selectfiles
    # Process file selection options
    if opts.select_from_file and \
           (opts.select or opts.select_cpp or opts.ignore):
        parser.error("you cannot use select-from-file and other "
                     "select options together.")




    # Compile regular expression
    try:
        regexp = re.compile(regexp)
    except re.error:
        parser.error("Cannot compile given regexp.")


    return regexp, sedcmd, roots, opts


#-------------------------------------------------------------------------------
#
def findgrepsed_main():
    """
    Main program for find-grep-sed script.
    """
    regexp, sedcmd, roots, opts = parse_options()





FIXME move to selectfiles

    # Process all files if no filter specified
    if not opts.select:
        opts.select = ['.*']

    # Create an appropriate generator for the "select" method
    if not opts.select_from_file:
        try:
            select = map(re.compile, opts.select)
            ignore = map(re.compile, opts.ignore)
        except re.error:
            raise SystemExit("Error: compiling select/ignore regexps.")

        selector = select_patterns(roots, select, ignore)
    else:
        selector = select_from_file(opts.select_from_file)







    # Perform search and replacement
    for fn in selector:
        check_replace(fn, regexp, sedcmd, opts)

    # Print out location of backup files again for convenience.
    if opts.backup_type == 'other' and opts.backup_dir:
        print
        print "Storing backup files under:", opts.backup_dir
        print


#-------------------------------------------------------------------------------
#
def main():
    xxdiff.scripts.interruptible_main(findgrepsed_main)

if __name__ == '__main__':
    main()
    
