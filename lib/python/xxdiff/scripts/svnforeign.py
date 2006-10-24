#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

# If this file is found independently as svn-foreign, here are the licensing
# terms:
#
#  svn-foreign
#  Copyright (C) 2006  Martin Blais
#  URL:  http://furius.ca/xxdiff/
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

"""svn-foreign [<options>] [<dir> ...]

This script deals with the common case where you have developed something using
Subversion and you have forgotten some unregistered files before committing.

svn-foreign runs 'svn status' on the given Subversion-managed directories, to
find out which files are unaccounted for and for each of these files, it asks
you what to do with it::

   [a] add it
   [d] delete it
   [i] ignore/mask it (sets an svn:ignore property on its parent directory)
   [s] skip it for now (leave it where it is)

Other actions::

   [q] quit / [x] exit, this interrupts the process
   [D] delete with no backups (for large files)
   [v] view the file with a pager (more)

The script works interactively and is meant to allow you to quickly deal with
the forgotten files in a subversion checkout.  It works with directories as
well.

"""
# Notes
# -----
# We need to make sure that this file remains independently working from the
# rest of xxdiff, i.e. even though some features may not be available, it should
# always be working without xxdiff.  This is because this file may be
# distributed under the simple name svn-foreign, and run independently, even if
# you have not the xxdiff infrastructure.
#
# Future Work
# -----------
#
# - We could implement backups for the deleted files (with lazy creation of
#   backup directory).
#
# Credits
# -------
# * To Sean Reifschneider (Jafo) for providing example code for raw tty input


__version__ = '$Revision$'
__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports
import sys, os, termios, tty, tempfile, datetime
from subprocess import Popen, PIPE, call
from os.path import *

# xxdiff imports (optional)
try:
    from xxdiff import backup
except ImportError:
    backup = None

    
#-------------------------------------------------------------------------------
#
debug = False

#-------------------------------------------------------------------------------
#
def read_one():
    """
    Reads a single character from the terminal.
    """
    # Set terminal in raw mode for faster input.
    orig_term_attribs = termios.tcgetattr(sys.stdin.fileno())
    tty.setraw(sys.stdin.fileno())
    try:
        c = sys.stdin.read(1)
        if c == chr(3): # INTR
            raise KeyboardInterrupt
    finally:
        termios.tcsetattr(sys.stdin.fileno(), termios.TCSAFLUSH,
                          orig_term_attribs)
    return c

#-------------------------------------------------------------------------------
#
def add(fn):
    """
    Add the file into Subversion.
    """
    call(['svn', 'add', fn])

#-------------------------------------------------------------------------------
#
def rmrf(fnodn):
    """
    Delete the given directory and all its contents.
    """
    if not exists(fnodn):
        return

    if isdir(fnodn):
        for root, dirs, files in os.walk(fnodn, topdown=False):
            for fn in files:
                os.remove(join(root, fn))
            for dn in dirs:
                adn = join(root, dn)
                if islink(adn):
                    os.remove(adn)
                else:
                    os.rmdir(adn)
        os.rmdir(root)
    else:
        os.remove(fnodn)

#-------------------------------------------------------------------------------
#
def ignore_prop(dn, ignores):
    """
    Set svn:ignore on directory 'dn'.
    """
    f = tempfile.NamedTemporaryFile('w')
    f.write(ignores)
    f.flush()
    call(['svn', 'propset', 'svn:ignore', '-F', f.name, dn])
    f.close()


#-------------------------------------------------------------------------------
#
def filter2(predicate, *arguments):
    ein, eout = [], []
    for args in zip(*arguments):
        add_args = args
        if len(args) == 1:
            add_args = args[0]

        if predicate(*args):
            ein.append(add_args)
        else:
            eout.append(add_args)
    return ein, eout


#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    parser.add_option('-c', '--commit', action='store_true',
                      help="Ask to commit after running.")

    parser.add_option('-m', '--comments', action='store',
                      help="Specify comments for commit.")

    parser.add_option('-v', '--verbose',
                      action='store_const', const=2, dest='verbose',
                      default=1,
                      help=optparse.SUPPRESS_HELP)
    parser.add_option('-q', '--quiet', '--silent', 
                      action='store_const', const=0, dest='verbose',
                      help="Suppress certain harmless warnings.")

    # Add backups disabling option if the xxdiff libraries are available.
    if backup is not None:
        backup.options_graft(parser,
                             "These options affect automatic backup of "
                             "deleted files.")
    
    opts, args = parser.parse_args()

    if backup is not None:
        backup.options_validate(opts, parser)

        if opts.backup_prefix is None:
            # Set a better default for the backup prefix.
            opts.backup_prefix = (
                datetime.datetime.now().replace(microsecond=0).isoformat('T'))

    if opts.comments and not opts.commit:
        parser.error("You cannot specify comments if you're not going to "
                     "commit!")

    return opts, args


#-------------------------------------------------------------------------------
#
def query_unregistered_svn_files(filenames, opts, output=sys.stdout,
                                  ignore=[]):
    """
    Runs an 'svn status' command, and then loops over all the files that are not
    registered, asking the user one-by-one what action to take (see this
    module's docstring for more details).

    'ignore' specifies a list of files to completely ignore.

    Return True upon completion.  Anything else signals that the user has
    interrupted the procedure.
    """
    write = output.write

    if not hasattr(opts, 'verbose'):
        # svnforeign requires a verbose option.
        raise RuntimeError(
            "Internal Error: You are missing a 'verbose' option.")

    ignore = map(abspath, ignore)

    # If there are multiple args--such as *--filter out the non svn directories.
    dirs, files = filter2(isdir, filenames)
    if len(dirs) > 1:
        svndirs = [x for x in dirs if isdir(join(x, '.svn'))]
        if svndirs != dirs:
            if opts.verbose >= 1:
                for dn in set(dirs) - set(svndirs):
                    write("Warning: skipping non-checkout dir '%s'\n" % dn)
            filenames = svndirs + files

    # Run svn status to find the files that are unaccounted for
    cmd = ['svn', 'status'] + filenames
    if debug:
        write(' '.join(cmd))
    p = Popen(cmd, stdout=PIPE, stderr=PIPE)
    out, err = p.communicate()
    if p.returncode != 0:
        raise SystemExit("Error: %s" % err)

    # Get out if there is nothing to do.
    if not out.strip():
        write('(Nothing to do.)\n')
        return True

    # Print the output of the status command for the user's excitement
    write('Status\n------\n')
    write(out)
    write('\n')

    # Process foreign files
    for line in out.splitlines():
        if line[0] == '?':
            fn = line[7:]
            if abspath(fn) in ignore:
                continue

            # Get the file size.
            try:
                size = os.lstat(fn).st_size
            except OSError, e:
                if e.errno == 2:
                    continue # File not found, was probably a temp file.
                else:
                    raise

            ftype = ''
            if islink(fn):
                ftype = '(symlink)'

            # Command loop (one command)
            while True:
                write('=> [Add|Del|Mask|Ign|View|Quit]  %10d  %s %s ? ' %
                      (size, fn, ftype))

                # Read command
                c = read_one()
                write(c)
                write('\n')

                if c == 'a': # Add
                    add(fn)
                    break

                elif c == 'd': # Delete
                    # Do backups if requested.
                    if backup is not None:
                        backupfn = backup.backup_file(fn, opts, output)
                        if backupfn:
                            write("Backed up to: '%s'\n" % backupfn)
                    elif opts.verbose >= 1:
                        write('(Warning: no backups.)\n')
                    rmrf(fn)
                    break

                elif c == 'D': # Delete with no backups
                    rmrf(fn)
                    break

                elif c in ['m', 'i', 'I']: # Ignore (Mask, svn:ignore)
                    dn, bn = split(fn)
                    if dn == '':
                        dn = '.'

                    # Get the properties first
                    p = Popen(['svn', 'propget', 'svn:ignore', dn],
                              stdout=PIPE, stderr=PIPE)
                    svnign, err = p.communicate()
                    if p.returncode != 0:
                        raise SystemExit(err)

                    # Sanitize the svn:ignore contents a bit
                    if svnign:
                        svnign = svnign.rstrip() + '\n'

                    write("--(current svn:ignore on '%s')--\n" % dn)
                    write(svnign)
                    write('----------------------------\n')

                    write("Add pattern (! to cancel, * to edit, default: [%s]):"
                          % bn)
                    pat = raw_input()
                    if pat == '':
                        pat = bn

                    if pat == '!':
                        print '(cancelled.)'

                    elif pat == '*':
                        call(['svn', 'propedit', 'svn:ignore', dn])

                    else:
                        svnign += pat + '\n'
                        ignore_prop(dn, svnign)

                        write('----------------------------\n')
                        write(svnign)
                        write('----------------------------\n')

                        break

                elif c == 's': # Skip
                    break

                elif c in ['q', 'x']: # Quit/exit
                    write('(Quitting.)\n')
                    return False

                elif c in 'v': # View
                    # Call on 'more' to view the file
                    write('-' * 80 + '\n')
                    pager = os.environ.get('PAGER', '/bin/more')
                    call([pager, fn])
                    # Look again

                elif c == chr(12): # Ctrl-L: Clear
                    # FIXME: is there a way to do this directly on the terminal?
                    call(['clear'])

                else: # Loop again
                    write('(Invalid answer.)\n')

    write('(Done.)\n')
    return True

    
#-------------------------------------------------------------------------------
#
def main():
    """
    Main program.
    """
    opts, args = parse_options()

    # Run the main loop...
    if query_unregistered_svn_files(args, opts, sys.stdout) is not True:
        # The user has quit.
        sys.exit(1)

    # Commit the files if requested.
    if opts.commit is not None:
        # Command loop (one command)
        sys.stdout.write('Commit? [Yes|No/Quit] ')

        # Read command
        c = read_one().lower()
        print c

        if c == 'y': # Commit.
            comments = opts.comments or ''
            call(['svn', 'commit', '-m', comments] + args)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print
        sys.exit(1)


