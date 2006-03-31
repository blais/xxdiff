#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xxdiff-cond-replace [<options>] <orig-file> <modified-file>

Useful script to conditionally replace a original by a generated file.

Basically, you invoke some script or pipeline of scripts to perform
modifications on an original text file and store the results in a temporary file
(the modified-file), and then invoke this script to conditionally overwrite the
contents of the orig-file with those of the modified file.

This works like copy (``cp``), except that xxdiff is shown to ask the user
whether to copy the file or not.  This is useful to run in a loop, for example::

   for i in `*/*.xml` ; do
      cat $i | ...pipeline of cmds that modifies the file... > /tmp/out.xml
      # copy only if the user accepts or merges.
      xxdiff-cond-replace /tmp/out.xml $i
   done

**IMPORTANT**: Notice well that the original file which will be overwritten is
on the RIGHT side. The syntax is like that of the UNIX cp command.

Exit Status
-----------

This program exits with status 0 if the copy operation was accepted or merged,
with 1 otherwise.

Notes
-----

- the script automatically creates backup files.

- the script automatically generates a detailed log of its actions and
  a text summary of all the differences beteween the original and new files.

- the script can optionally checkout the file before performing the replacement.

"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.3']


# stdlib imports.
import sys, os
from os.path import *
import commands, tempfile, shutil

# xxdiff imports.
import xxdiff.scripts
import xxdiff.backup
import xxdiff.scm
import xxdiff.invoke
from xxdiff.scripts import tmpprefix



FIXME remove this, 

#-------------------------------------------------------------------------------
#
def do_replace_file( ofn, nfn, opts, oss ):
    """
    Function that performs the file replacement safely.  Replace the original
    file 'ofn' by 'nfn'.
    """
    # Backup the original file first.
    xxdiff.backup.backup_file(ofn, opts, oss)

    # Insure that the file is checked out.
    xxdiff.scm.insure_checkout(ofn, opts, oss)

    # Copy the new file over the original.
    shutil.copyfile(nfn, ofn)

#-------------------------------------------------------------------------------
#
def cond_replace( origfn, newfn, opts ):
    """
    Spawn xxdiff and perform the replacement if confirmed.
    """

    # Print header
    if opts.verbose >= 2:
        print '=' * 80
        print 'File:    ', origfn
        print 'Absolute:', abspath(origfn)
        print

    #===========================================================================
    # BEGIN FILE TRANSFORMATION

    # Nothing to do.

    # END FILE TRANSFORMATION
    #===========================================================================

    if opts.verbose >= 2:
        # Run diff between the original and the new/modified file
        difffmt = 'diff -y --suppress-common-lines "%s" "%s" 2>&1'
        diffcmd = difffmt % (origfn, newfn)
        status, diff_output = commands.getstatusoutput(diffcmd)
        if os.WEXITSTATUS(status) == 0:
            print
            print "(Warning: no differences.)"
            print

        # Print output from sed command
        print diff_output
        print

    # Return immediately if this is not for real
    if opts.dry_run:
        return 0

    def print_decision( decision ):
        # Print out decision code.
        if opts.verbose >= 2:
            print decision
        elif opts.verbose >= 1:
            print '%10s %s' % (decision, origfn)

    returnval = 0
    if opts.no_confirm:
        # No graphical diff, just replace the files without asking.
        do_replace_file(origfn, newfn, opts, sys.stdout)

        print_decision('NOCONFIRM')
    else:
        # Call xxdiff!
        decision, mergedf = xxdiff.invoke.xxdiff_decision(
            opts, '--title2', 'NEW FILE', origfn, newfn)

        print_decision(decision)

        if decision == 'ACCEPT':
            # Changes accepted, replace original with new..
            do_replace_file(origfn, newfn, opts, sys.stdout)

        elif decision == 'REJECT' or decision == 'NODECISION':
            # Rejected change (or program killed), do not replace
            returnval = 1

        elif decision == 'MERGED':
            if opts.verbose >= 2:
                # Run diff again to show the real changes that will be applied.
                diffcmd2 = difffmt % (origfn, mergedf.name)
                status, diff_output = commands.getstatusoutput(diffcmd2)
                print 'Actual merged changes:'
                print
                print diff_output
                print

            do_replace_file(origfn, mergedf.name, opts, sys.stdout)

    return returnval

























#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse

    parser = optparse.OptionParser(__doc__.strip())

    xxdiff.backup.options_graft(parser)
    xxdiff.scm.options_graft(parser)
    xxdiff.invoke.options_graft(parser)

    parser.add_option('-n', '--dry-run', action='store_true',
                      help="print the commands that would be executed " +
                      "but don't really run them.")

    parser.add_option('-v', '--verbose', '--quiet', action='count',
                      default=0,
                      help="Verbose output.  By default nothing is output. "
                      "If you use this switch once, you will get the decision "
                      "code followed by the filename.  If you use it twice, "
                      "you will obtain a side-by-side diff of the changes.")

    parser.add_option('-x', '--diff', action='store_true',
                      help="Run a diff and log the differences on stdout.")

    parser.add_option('-d', '--delete', action='store_true',
                      help="Instead of copying the temporary file, move it "
                      "(delete it after copying).")

    parser.add_option('-X', '--no-confirm', action='store_true',
                      help="do not ask for confirmation with graphical "
                      "diff viewer. This essentially generates a diff log and "
                      "copies the file over with backups.")

    xxdiff.scripts.install_autocomplete(parser)
    opts, args = parser.parse_args()

    xxdiff.backup.options_validate(opts, logs=sys.stdout)
    xxdiff.scm.options_validate(opts)
    xxdiff.invoke.options_validate(opts)

    if not args or len(args) > 2:
        raise parser.error("you must specify exactly two files.")

    return opts, args


#-------------------------------------------------------------------------------
#
def condreplace_main():
    """
    Main program for cond-replace script.
    """
    opts, args = parse_options()

    if len(args) == 1:
        if opts.delete:
            raise parser.error("no need to use --delete on file from stdin.")

        origfn, = args
        intmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)
        try:
            intmpf.write(sys.stdin.read())
            sys.stdin.close()
            intmpf.flush()
        except IOError, e:
            raise SystemExit(
                "Error: saving stdin to temporary file (%s)" % str(e))
        newfn = intmpf.name
    else:
        newfn, origfn = args

    # call xxdiff and perform the conditional replacement.
    rval = cond_replace(origfn, newfn, opts)

    if opts.delete and not opts.dry_run:
        try:
            os.unlink(newfn)
        except OSError, e:
            raise SystemExit("Error: deleting modified file (%s)" % str(e))

    # repeat message at the end for convenience.
    if opts.backup_type == 'other' and opts.backup_dir:
        print
        print "Storing backup files under:", opts.backup_dir
        print

    return rval


#-------------------------------------------------------------------------------
#
def main():
    xxdiff.scripts.interruptible_main(condreplace_main)

if __name__ == '__main__':
    main()

