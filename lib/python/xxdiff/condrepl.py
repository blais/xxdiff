#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Function for conditional replacement.

"""

__author__ = "Martin Blais <blais@furius.ca>"


# stdlib imports.
import sys, os
from os.path import *
from subprocess import Popen, PIPE
import shutil

# xxdiff imports.
import xxdiff.backup
import xxdiff.invoke
import xxdiff.scm


__all__ = ('cond_replace',)


#-------------------------------------------------------------------------------
#
# Side-by-side diff command.
sbs_diff_cmd = ['diff', '--side-by-side', '--suppress-common-lines']

#-------------------------------------------------------------------------------
#
def cond_replace( origfn, newfn, opts, logs, exitonsame=False ):
    """
    Given two filenames, spawn xxdiff for a decision, and conditionally replace
    the original file with the contents of the new file.  This supports options
    for backups and SCM checkout.

    Arguments:

    - 'origfn': filename to process -> string

    - 'newfn': the new filename to compare -> string

    - 'opts': program options object -> Options instance

      The options that are used are:

      * opts.verbose: How much output to print

          Three levels of verbosity are assumed:
          - 0: completely silent
          - 1: outputs only the decision code and then the filename
               (This can be useful for scripts to process the output of a run)
          - 2: outputs a side-by-side diff of the changes.

      * opts.dry_run: Whether to actually apply the changes or not.

      * opts.no_confirm: Apply the changes without confirmation.

    - 'exitonsame': do not even pop xxdiff if the files are the same

    Returns the decision code for the file.
    """

    # A handy close for printing out the decision code.
    def print_decision( decision ):
        if opts.verbose >= 2:
            print >> logs, decision
        elif opts.verbose >= 1:
            print >> logs, '%-10s %s' % (decision, origfn)

    def print_diffs( diff_output ):
        print >> logs
        for line in map(str.expandtabs, diff_output.splitlines()):
            print >> logs, ' |', line
        print >> logs


    origfn = normpath(abspath(origfn))

    # Print header
    if opts.verbose >= 2:
        print >> logs,  '=' * 80
        print >> logs,  'File: ', origfn

    if opts.verbose >= 2 or exitonsame:

        # Run diff between the original and the new/modified file
        p = Popen(sbs_diff_cmd + [origfn, newfn], stdout=PIPE, stderr=PIPE)
        diff_output, stderr = p.communicate()

        # Print differences.
        if p.returncode == 0:
            if opts.verbose >= 2:
                print >> logs, "(Warning: no differences.)"

            if exitonsame:
                print_decision('NODIFF')
                return 0
    else:
        diff_output = None

    if opts.no_confirm:
        # No graphical diff, just replace the files without asking.
        do_replace_file(origfn, newfn, opts, logs)
        decision = 'NOCONFIRM'

        print_decision(decision)
        if opts.verbose >= 2 and diff_output: print_diffs(diff_output)
    else:
        # Call xxdiff!
        decision, mergedf = xxdiff.invoke.xxdiff_decision(
            opts, '--title2', 'NEW FILE', origfn, newfn)

        print_decision(decision)

        if decision == 'ACCEPT':
            # Changes accepted, replace original with new..
            do_replace_file(origfn, newfn, opts, logs)

            if opts.verbose >= 2 and diff_output: print_diffs(diff_output)

        elif decision == 'REJECT' or decision == 'NODECISION':
            # Rejected change (or program killed), do not replace
            pass

        elif decision == 'MERGED':
            if opts.verbose >= 2:
                # Run diff again to show the real changes that will be applied.
                p = Popen(sbs_diff_cmd + [origfn, mergedf.name],
                          stdout=PIPE, stderr=PIPE)
                diff_output, stderr = p.communicate()

                if diff_output: print_diffs(diff_output)

            do_replace_file(origfn, mergedf.name, opts, logs)

    return decision


#-------------------------------------------------------------------------------
#
def do_replace_file( ofn, nfn, opts, logs ):
    """
    Function that performs the file replacement safely.  Replace the original
    file 'ofn' by 'nfn'.
    """
    if opts.dry_run:
        return

    # Backup the original file first.
    if hasattr(opts, 'backup_type'):
        xxdiff.backup.backup_file(ofn, opts, logs)

    # Insure that the file is checked out.
    if hasattr(opts, 'checkout'):
        xxdiff.scm.insure_checkout(ofn, opts, logs)

    # Copy the new file over the original.
    shutil.copyfile(nfn, ofn)


#-------------------------------------------------------------------------------
#
def test():
    """
    Simple interactive test.
    """
    import tempfile
    from pprint import pprint

    newdir = tempfile.mkdtemp()
    fm = dict(map(lambda x: ('file%d' % x, join(newdir, 'file%d' % x)),
                  xrange(1, 5)))
    pprint(fm)
    os.system('cp /home/blais/.emacs %(file1)s' % fm) # TEST
    os.system('cat %(file1)s | sed -e "s/contr/FREAK/" > %(file2)s' % fm) # TEST

    class Opts:
        "dummy options class"
        verbose = 2
        dry_run = False
        no_confirm = False

    print cond_replace(fm['file1'], fm['file2'], Opts, sys.stdout)
    pprint(fm)



if __name__ == '__main__':
    test()

