# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to backup files before overwriting them.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import sys, os, shutil, optparse, tempfile
from os.path import *

# xxdiff imports.
from xxdiff.scripts import tmpprefix


# Different kinds of backups.
backup_choices = ['along', 'other', 'none']

def options_graft(parser, msg=None, deftype='other'):
    """
    Graft options on given parser for automatic file backups.
    """
    if msg is None:
        msg = "These options affect automatic backup of overwritten files."

    group = optparse.OptionGroup(parser, "File backup options", msg)

    group.add_option('-b', '--backup-type', action='store',
                     type='choice', choices=backup_choices,
                     default=deftype, metavar="CHOICE",
                     help=("Selects the backup type from: %s (default: %s)" %
                           (', '.join(backup_choices), deftype)))

    group.add_option('--backup-dir', action='store',
                     help="Specify backup directory for type 'other'")

    group.add_option('--backup-prefix', action='store', default=None,
                     help=optparse.SUPPRESS_HELP)

    parser.add_option_group(group)

    return group


def options_validate(opts, parser, logs=None):
    """
    Validate backup options.
    """
    if opts.backup_type != 'other' and opts.backup_dir:
        parser.error(
            "option backup-dir is only valid for backups of type 'other'.")


def backup_file(fn, opts, logs=None):
    """
    Compute backup filename and copy backup file.

    Arguments:
    - 'fn': filename to backup -> string
    - 'opts': program options -> Options instance
    """
    if hasattr(logs, 'write'):
        logs = logs.write
        
    # Compute destination backup filename
    if opts.backup_type == 'along':
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
        # Always backup the files rooted to the very root, because there may be
        # multiple root directories during the selection process.  Besides, it
        # does not matter all that much, the backup files are not really meant
        # to be accessed unless something goes really wrong.
        relfn = abspath(fn)

        # Remove the leading slash (this may f*ck up under Windoze).
        relfn = relfn[1:]

        # If the backup directory has not been set, create a temporary one
        if opts.backup_dir is None:
            if opts.backup_prefix:
                pfx = opts.backup_prefix + '.'
            else:
                pfx = ''
            opts.backup_dir = tempfile.mkdtemp(prefix=tmpprefix + pfx)
        # Otherwise create the specified directory if it does not exist
        elif not exists(opts.backup_dir):
            os.makedirs(opts.backup_dir)

        backupfn = join(opts.backup_dir, relfn)

        if opts.verbose >= 0 and exists(backupfn) and logs:
            logs("(Warning: Overwriting existing file in backup '%s')\n" %
                 backupfn)
        
    else: # opts.backup_type == 'none'
        backupfn = None

    if backupfn:
        # Perform the backup
        if logs and opts.verbose >= 3:
            logs('Backup: %s\n' % backupfn)

        # Make sure that the destination directory exists
        ddn = dirname(backupfn)
        if ddn and not exists(ddn):
            os.makedirs(ddn)

        # Copy the original to the backup directory
        if isfile(fn) or islink(fn):
            shutil.copy2(fn, backupfn)
        elif isdir(fn):
            shutil.copytree(fn, backupfn, True)
        else:
            raise RuntimeError("Internal error: cannot backup '%s'. " % fn,
                               "Invalid file type.")

    return backupfn


def print_reminder(opts):
    """
    Print a reminder of the location of the backup files.
    """
    # Print reminder of location of backup files at the end (for convenience).
    if opts.backup_dir:
        print
        print "Backup files in: '%s'" % opts.backup_dir
        print


def test():
    """
    Test stub.
    """
    parser = optparse.OptionParser(__doc__.strip())
    options_graft(parser)
    opts, args = parser.parse_args()

    options_validate(opts, parser, logs=sys.stdout)

    print 'Dir:', opts.backup_dir
    print 'Type:', opts.backup_type

    backup_file('/home/blais/.emacs', opts, sys.stdout)


        
if __name__ == '__main__':
    test()

