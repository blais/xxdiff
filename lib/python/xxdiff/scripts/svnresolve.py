# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xx-svn-resolve [<options>] <file> <file> [<file>]

Script that invokes xxdiff with a 3-way merge view for all files in a Subversion
checkout whose conflicts need to be resolved.  Depending on the exit status,
xxdiff also calls 'svn resolve' on the files.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4', 'Subversion']


# stdlib imports.
import sys, os, re
from os.path import *

# xxdiff imports.
import xxdiff.scripts
import xxdiff.backup
import xxdiff.condrepl
from xxdiff.scm import subversion
from xxdiff.scripts.encrypted import diff_encrypted


def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    xxmodules = (xxdiff.backup,)
    for mod in xxmodules:
        mod.options_graft(parser)

    parser.add_option('-n', '--dry-run', action='store_true',
                      help="Print the commands that would be executed " +
                      "but don't really run them.")

    parser.add_option('-R', '--no-resolve', '--dont-resolve',
                      action='store_true',
                      help="Do not resolve the conflicts even after a merge "
                      "decision has been made.")

    # Some of the GPG options duplicated here.
    parser.add_option('-g', '--gpg', default="gpg",
                      help="Specify path to gpg program to use.")
    parser.add_option('-A', '--dont-armor', action='store_true',
                      help="Create output file in binary format.")
    parser.add_option('-r', '--recipient', action='store',
                      help="Encrypt for user id name.")

    xxdiff.scripts.install_autocomplete(parser)
    opts, args = parser.parse_args()

    # Force to always perform a verbose diff on output.
    opts.verbose = 2

    for mod in xxmodules[1:]:
        mod.options_validate(opts, parser)

    return opts, args


def select_conflicts(statii):
    """
    A generator that selects only the conflictual files.
    """
    for s in statii:
        if s.status == 'C':
            yield s

def svnresolve_main():
    """
    Main program for svn-resolve script.
    """
    opts, args = parse_options()

    # Get the status of the working copy.
    statii = subversion.status(args)

    # First print out the list/status of the conflicting files to the user.
    for s in select_conflicts(statii):
        print s.parsed_line
    print

    logs = sys.stdout

    # For each of the files reported by status
    for s in select_conflicts(statii):

        # Get the three files before the merge conflicts.
        info = subversion.getinfo(s.filename)
        dn = dirname(s.filename)
        ancestor, mine, yours = [join(dn, info[x]) for x in
                                 ('Conflict Previous Base File',
                                  'Conflict Previous Working File',
                                  'Conflict Current Base File')]

        # Spawn xxdiff in decision mode on the three files. We dispatch to the
        # encrypted version if necessary.
        if re.match('.*\.asc', s.filename):
            tmine = open(mine).read()
            tancestor = open(ancestor).read()
            tyours = open(yours).read()
            decision = diff_encrypted([tmine, tancestor, tyours], opts, outmerged=s.filename)
        else:
            decision = xxdiff.condrepl.cond_resolve(
                mine, ancestor, yours, s.filename, opts, logs, extra=('--merge',))

        # Backup all the other files that will get when the file gets resolved,
        # whether by this script or later by the user by hand.
        if decision in xxdiff.condrepl.proper_decisions:
            xxdiff.backup.backup_file(mine, opts, logs)
            xxdiff.backup.backup_file(ancestor, opts, logs)
            xxdiff.backup.backup_file(yours, opts, logs)
        else:
            # If no proper decision has been made, do not resolve.
            continue

        # Resolve the conflict with Subversion, if requested.
        if not opts.no_resolve:
            subversion.resolve(s.filename)
            print '(Resolved.)'

    xxdiff.backup.print_reminder(opts)


def main():
    xxdiff.scripts.interruptible_main(svnresolve_main)

if __name__ == '__main__':
    main()


