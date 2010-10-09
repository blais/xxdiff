# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xx-match [<options>] <file> <file> <file> <file> ...

Wrapper script for xxdiff that implements an invocation syntax like this:

  xx-match dir1/*.c dir2/*.c

The way we implement the heuristic to match up files is simple: we match up the
basenames of the files.  If there are exactly two or three files in the list, we
invoke an xxdiff on these files.  The files are always invoked in the order that
they show up on the command-line.

Another way to achieving similar behaviour is to use a more generic file
matching script (see 'match-files' by the same author) along with xargs, e.g.::

  match-files dir1/*.c dir2/*.c | xargs -n2 xxdiff

Using xx-match has the advantage that it work with matches of 2 or 3 files,
and also spawns xxdiff on single files (with an empty side).

Note: many users do not seem to understand why such a script is necessary. One
must realize a few things: 1) globbing pattern expansion is done by the shell,
before invocation of xxdiff, thus xxdiff cannot be aware of the globbing
patterns (unless it itself would support the heuristic mentioned here, along
with the user quoting the globbing patterns to avoid shell expansion); 2) the
solution to this problem using expanded file lists is not trivial, the input
(and its ordering) can encompass a lot more possibilities than immediately
appears. Nevertheless, we built this functionality/heuristic in this script,
because it may be useful to some.

See 'match-files' script by the same author for a more generic implementation of
this file matching heuristic.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4']


# stdlib imports.
import sys, os

# xxdiff imports.
import xxdiff.scripts
import xxdiff.invoke


def match_main():
    """
    Main program for match script.
    """
    # Get options
    opts, files = xxdiff.scripts.passthruopts(sys.argv)

    # Make sure that we display the commands.
    class Opts:
        xxdiff_verbose = True

    # Build map of basenames
    bnmap = {}
    for fn in files:
        dn, bn = os.path.split(fn)
        bnmap.setdefault(bn, []).append(fn)

    # Invoke xxdiff's on alphabetical order of the basenames
    bnkeys = bnmap.keys()
    bnkeys.sort()
    for bn in bnkeys:
        filenames = bnmap[bn]
        if len(filenames) == 2 or len(filenames) == 3:
            extra = []
        elif len(filenames) == 1:
            extra = ['--single']
        else:
            # ignore the files.
            continue

        xxdiff.invoke.xxdiff_display(Opts, *(extra + filenames))


def main():
    xxdiff.scripts.interruptible_main(match_main)

if __name__ == '__main__':
    main()

