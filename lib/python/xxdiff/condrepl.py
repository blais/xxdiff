# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions for conditional replacement.
"""

__author__ = "Martin Blais <blais@furius.ca>"


# stdlib imports.
import sys, os, optparse
from os.path import *
from subprocess import Popen, PIPE
import shutil

# xxdiff imports.
import xxdiff.backup
import xxdiff.invoke
import xxdiff.checkout


__all__ = ('cond_replace',)


def options_graft(parser):
    """
    Graft options on given parser for automatic file backups.
    """
    group = optparse.OptionGroup(parser, "Replacement options",
                                 "These options affect the overwriting of the"
                                 "original file.")

    group.add_option('-n', '--dry-run', action='store_true',
                     help="Print the commands that would be executed " +
                     "but don't really run them.")

    group.add_option('-x', '--no-confirm', action='store_true',
                     help="Do not ask for confirmation with graphical "
                     "diff viewer.")

    parser.add_option_group(group)

    return group


def options_validate(opts, parser, logs=None):
    """
    Validate replacement options.
    """
    pass


# Side-by-side diff2 command.
sbs_diff_cmd = ['diff', '--side-by-side', '--suppress-common-lines']

def cond_replace(origfn, newfn, opts, logs, exitonsame=False):
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
                print_decision('NODIFF', origfn, opts, logs)
                return 'NODECISION'
    else:
        diff_output = None

    if opts.no_confirm:
        # No graphical diff, just replace the files without asking.
        do_replace_file(origfn, newfn, opts, logs)
        decision = 'NOCONFIRM'

        print_decision(decision, origfn, opts, logs)
        if opts.verbose >= 2 and diff_output: print_diffs(diff_output, logs)
    else:
        # Call xxdiff!
        decision, mergedf, retcode = xxdiff.invoke.xxdiff_decision(
            opts, '--title2', 'NEW FILE', origfn, newfn)

        print_decision(decision, origfn, opts, logs)

        if decision == 'ACCEPT':
            # Changes accepted, replace original with new..
            do_replace_file(origfn, newfn, opts, logs)

            if opts.verbose >= 2 and diff_output: print_diffs(diff_output, logs)

        elif decision == 'REJECT' or decision == 'NODECISION':
            # Rejected change (or program killed), do not replace
            pass

        elif decision == 'MERGED':
            if opts.verbose >= 2:
                # Run diff again to show the real changes that will be applied.
                p = Popen(sbs_diff_cmd + [origfn, mergedf.name],
                          stdout=PIPE, stderr=PIPE)
                diff_output, stderr = p.communicate()

                if diff_output: print_diffs(diff_output, logs)

            do_replace_file(origfn, mergedf.name, opts, logs)

    return decision


def print_decision(decision, origfn, opts, logs):
    """
    Print the decision string.
    """
    if opts.verbose >= 2:
        print >> logs, decision
    elif opts.verbose >= 1:
        print >> logs, '%-10s %s' % (decision, origfn)

def print_diffs(diff_output, logs):
    """
    Format nicely and print the output of side-by-side diff.
    """
    print >> logs
    for line in map(str.expandtabs, diff_output.splitlines()):
        print >> logs, ' |', line
    print >> logs

def do_replace_file(ofn, nfn, opts, logs):
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
    if getattr(opts, 'checkout', None):
        xxdiff.checkout.insure_checkout(ofn, opts, logs)

    # Copy the new file over the original.
    shutil.copyfile(nfn, ofn)



diff3_cmd = ['diff3']

proper_decisions = ('ACCEPT', 'REJECT', 'MERGED')

def cond_resolve(mine, ancestor, yours, output, opts, logs=None, extra=None):
    """
    Given three filenames, 'mine', 'ancestor', 'yours', spawn a 3-way xxdiff for
    a decision, and replace the 'original' file with the contents of the merged
    output.

    Arguments:

    - 'mine', 'ancestor', 'yours': files that contains the local file before
      merging, the common ancestor file of 'mine' and 'yours', and 'yours' the
      merged file, -> strings

    - 'output': the filename where we want to store the merged results (in the
      case of a merge, the original filename) -> string

    - 'opts': program options object -> Options instance

      The options that are used are:

      * opts.verbose: How much output to print

          Three levels of verbosity are assumed:
          - 0: completely silent
          - 1: outputs only the decision code and then the filename
               (This can be useful for scripts to process the output of a run)
          - 2: outputs a side-by-side diff of the changes.

      * opts.dry_run: Whether to actually apply the changes or not.

    - 'extra': are extra parameters to pass on to xxdiff.
    
    Returns the decision code for the file.
    """
    mine, ancestor, yours, output = map(abspath,
                                        (mine, ancestor, yours, output))
    files3 = [mine, ancestor, yours]

    # Print header
    if opts.verbose >= 2:
        print >> logs,  '=' * 80
        print >> logs,  'File: ', output

    if opts.verbose >= 2:
        # Run diff between the three files.
        p = Popen(diff3_cmd + files3, stdout=PIPE, stderr=PIPE)
        diff_output, stderr = p.communicate()

        # Print differences.  
        if not diff_output: # Note: we cannot rely on the return code.
            if opts.verbose >= 2:
                print >> logs, "(Warning: no differences.)"

            print_decision('NODIFF', mine, opts, logs)
            return 'NODECISION'
    else:
        diff_output = None

    # Add arguments to identify files in the title bars.
    dargs = xxdiff.invoke.title_opts('%s (WORKING)' % mine,
                                     '%s (ANCESTOR)' % ancestor,
                                     '%s (MERGED/NEW BASE)' % yours)
    if extra is not None:
        dargs = list(extra) + dargs

    # Call xxdiff!
    dargs.extend(files3)
    decision, mergedf, retcode = xxdiff.invoke.xxdiff_decision(opts, *dargs)

    print_decision(decision, mine, opts, logs)

    # Backup all 3 files and the destination output before overwriting.
    if decision != 'NODECISION' and not opts.dry_run:
        if hasattr(opts, 'backup_type'):
            xxdiff.backup.backup_file(output, opts, logs)

    if decision == 'ACCEPT':
        # Accept: discard our local changes and replace file with the update.
        shutil.copyfile(yours, output)

    elif decision == 'REJECT':
        # Reject: discard the update and keep our local file as it were before
        # the merge.
        shutil.copyfile(mine, output)

    elif decision == 'MERGED':
        # Merge: replace the output file with the merged output of xxdiff.
        shutil.copyfile(mergedf.name, output)

    elif decision == 'NODECISION':
        # No Decision: Do not change anything.
        pass

    if opts.verbose >= 2 and decision in proper_decisions:
        # Run diff again to show the real changes that will be applied to
        # 'mine' into the output file.
        p = Popen(sbs_diff_cmd + [mine, output], stdout=PIPE, stderr=PIPE)
        diff_output, stderr = p.communicate()

        if diff_output:
            print_diffs(diff_output, logs)

    return decision


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

