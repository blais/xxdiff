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
__depends__ = ['xxdiff', 'Python-2.4']


# stdlib imports.
import sys, os
from os.path import *
import tempfile

# xxdiff imports.
import xxdiff.scripts
import xxdiff.backup
import xxdiff.checkout
import xxdiff.invoke
import xxdiff.condrepl
from xxdiff.scripts import tmpprefix


#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse

    parser = optparse.OptionParser(__doc__.strip())

    xxmodules = xxdiff.backup, xxdiff.checkout, xxdiff.invoke, xxdiff.condrepl
    for mod in xxmodules:
        mod.options_graft(parser)

    parser.add_option('-v', '--verbose', action='count',
                      default=0,
                      help="Verbose output.  By default nothing is output. "
                      "If you use this switch once, you will get the decision "
                      "code followed by the filename.  If you use it twice, "
                      "you will obtain a side-by-side diff of the changes.")

    parser.add_option('-d', '--diff', action='store_true',
                      help="Run a diff and log the differences on stdout.")

    parser.add_option('--delete', action='store_true',
                      help="Instead of copying the temporary file, move it "
                      "(delete it after copying).")

    parser.add_option('-D', '--exit-on-same', action='store_true',
                      help="Do not do anything if the files are the same.")

    xxdiff.scripts.install_autocomplete(parser)
    opts, args = parser.parse_args()

    for mod in xxmodules:
        mod.options_validate(opts, parser, logs=sys.stdout)

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
    decision = xxdiff.condrepl.cond_replace(origfn, newfn, opts, sys.stdout,
                                            opts.exit_on_same)

    if opts.delete and not opts.dry_run:
        try:
            os.unlink(newfn)
        except OSError, e:
            raise SystemExit("Error: deleting modified file (%s)" % str(e))

    # repeat message at the end for convenience.
    if opts.backup_type == 'other' and opts.backup_dir:
        print
        print "  Backup files:", opts.backup_dir
        print

    # Compute return value.
    rval = 0
    if decision in ('REJECT', 'NODECISION'):
        rval = 1 

    return rval


#-------------------------------------------------------------------------------
#
def main():
    xxdiff.scripts.interruptible_main(condreplace_main)

if __name__ == '__main__':
    main()

