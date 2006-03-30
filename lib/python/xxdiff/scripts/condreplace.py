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
      xxdiff-cond-replace $i /tmp/out.xml
   done

**IMPORTANT**: Notice well that the original file which will be overwritten is
on the LEFT side. The syntax is the reverse of the UNIX cp command.

Exit Status
-----------

This program exits with status 0 if the copy operation was accepted or merged,
with 1 otherwise.

Notes
-----

- the script automatically creates backup files.
- the script automatically generates a detailed log of its actions and
  a text summary of all the differences beteween the original and new files.
- the script can optionally checkout the file with ClearCase before performing
  the replacement.

"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.3']


# stdlib imports.
import sys, os, re
from os.path import *
import commands, tempfile, shutil

# xxdiff imports.
import xxdiff.scripts
import xxdiff.backup
import xxdiff.checkout
from xxdiff.scripts import tmpprefix


#-------------------------------------------------------------------------------
#
def cond_replace( origfile, modfile, opts ):
    """
    Spawn xxdiff and perform the replacement if confirmed.
    """

    if opts.diff:
        print '=' * 80
        print 'File:    ', origfile
        print 'Absolute:', abspath(origfile)
        print

        # Run diff command.
        difffmt = 'diff -y --suppress-common-lines "%s" "%s" 2>&1'
        diffcmd = difffmt % (origfile, modfile)
        s, o = commands.getstatusoutput(diffcmd)

        rv = os.WEXITSTATUS(s)
        if rv == 0:
            print >> sys.stderr
            print >> sys.stderr, "Warning: no differences."
            print >> sys.stderr

        #print
        #print '_' * 80
        print o
        #print '_' * 80
        #print
        print

    rval = 0
    if not opts.dry_run:
        tmpf2 = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

        if not opts.no_confirm:
            cmd = ('%s --decision --merged-filename "%s" ' + \
                   '--title2 "NEW FILE" "%s" "%s" ') % \
                   (opts.command, tmpf2.name, origfile, modfile)
            s, o = commands.getstatusoutput(cmd)

        else:
            s, o = 0, 'NO_CONFIRM'

        if not opts.silent:
            if opts.diff:
                print o
            else:
                print o, origfile
        if o == 'ACCEPT' or o == 'NO_CONFIRM':
            xxdiff.backup.backup_file(origfile, opts, sys.stdout)

            xxdiff.checkout.insure_checkout(origfile, opts, sys.stdout)

            shutil.copyfile(modfile, origfile)
        elif o == 'REJECT' or o == 'NODECISION':
            rval = 1
        elif o == 'MERGED':
            if opts.diff:
                # run diff again to show the changes that have actually been
                # merged in the output log.
                diffcmd = difffmt % (origfile, tmpf2.name)
                s, o = commands.getstatusoutput(diffcmd)
                print 'Actual merged changes:'
                print
                print o
                print

            xxdiff.backup.backup_file(origfile, opts, sys.stdout)
            xxdiff.checkout.insure_checkout(origfile, opts, sys.stdout)

            shutil.copyfile(tmpf2.name, origfile)
        else:
            raise SystemExit("Error: unexpected answer from xxdiff: %s" % o)

        if opts.delete:
            try:
                os.unlink(modfile)
            except OSError, e:
                raise SystemExit("Error: deleting modified file (%s)" % str(e))

    return rval











#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse

    parser = optparse.OptionParser(__doc__.strip())

    xxdiff.backup.options_graft(parser)
    xxdiff.checkout.options_graft(parser)

    parser.add_option('--command', action='store', default='xxdiff',
                      help="xxdiff command prefix to use.")


    parser.add_option('-n', '--dry-run', action='store_true',
                      help="print the commands that would be executed " +
                      "but don't really run them.")

    parser.add_option('-q', '--silent', '--quiet', action='store_true',
                      help="Do not output anything. Normally the decision "
                      "status and backup file location is output.")

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
    xxdiff.checkout.options_validate(opts)

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

        origfile, = args
        intmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)
        try:
            intmpf.write(sys.stdin.read())
            sys.stdin.close()
            intmpf.flush()
        except IOError, e:
            raise SystemExit(
                "Error: saving stdin to temporary file (%s)" % str(e))
        modfile = intmpf.name
    else:
        origfile, modfile = args

    if opts.silent and opts.diff:
        raise parser.error("you cannot ask for a diff output and for silent at "
                           "the same time.")

    # call xxdiff and perform the conditional replacement.
    rval = cond_replace(origfile, modfile, opts)

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

