# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
xx-find-grep-sed [<options>] <regexp> <sed-cmd>

Perform global sed-like replacements in a set of files.

This program walks a directory hierarchy, selects some files to be processed by
filenames (or by default, process all files found), then each selected file is
searched for some regexp pattern, and if there is a match, run the given file
through a sed command and replace the file with that output.

This script effectively becomes obsolete, because it can be replace by a much
more generic equivalent::

   xx-filter --select-grep='<regexp>' 'sed -e <sed-code>'

"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4', 'diffutils', 'sed']


# stdlib imports.
import sys, os, re
from os.path import *
from subprocess import Popen, PIPE

# xxdiff imports.
import xxdiff.scripts
import xxdiff.xformloop


class GrepSedTransformer(xxdiff.xformloop.Transformer):
    """
    Transformer that greps the file for a regular expression, and if it matches,
    runs it through a sed command.
    """
    def __init__(self, opts, regexp, sedprog):
        xxdiff.xformloop.Transformer.__init__(self, opts)
        self.regexp, self.sedprog = regexp, sedprog

    def transform(self, fn, outf):
        # Grep the input file.
        try:
            text = open(fn, 'r').read()
        except IOError, e:
            # Don't bail out if we cannot read a file.
            print >> sys.stderr, "Warning: cannot read file '%s'." % fn
            return False

        # Grep the file.
        if not self.regexp.search(text, re.MULTILINE):
            return False

        # If this is not for show only, make sure right away that we can
        # overwrite the destination file
        if self.opts.dry_run and not os.access(fn, os.W_OK):
            raise SystemExit("Error: cannot write to file '%s'." % fn)

        # Perform sed replacement to a temporary file
        p = Popen(['sed', '-e', self.sedprog],
                  stdin=PIPE, stdout=outf, stderr=PIPE)
        try:
            stdout, stderr = p.communicate(text)
        except OSError, e:
            raise SystemExit("Error: running sed command '%s':\n%s" %
                             (self.sedprog, p.stderr.read()))

        if p.returncode != 0:
            raise SystemExit(
                "Error: running sed command:\nFile:%s\n%s" % (fn, stderr))

        return True


def parse_options():
    import optparse
    parser = optparse.OptionParser(__doc__.strip())
    opts, args, selector = xxdiff.xformloop.parse_args(parser)

    # Check that we got two arguments
    if len(args) < 2:
        parser.error("You must specify at least a regular expression and "
                     "a sed command")
    regexp, sedprog = args

    # Compile the regular expression
    try:
        regexp = re.compile(regexp)
    except re.error:
        parser.error("Cannot compile given regexp '%s'." % regexp)

    # Force to always perform a diff on output.
    opts.verbose = 2

    return regexp, sedprog, opts, selector


def findgrepsed_main():
    """
    Main program for find-grep-sed script.
    """
    # Parse the options.
    regexp, sedprog, opts, selector = parse_options()

    # Create an appropriate transformer.
    xformer = GrepSedTransformer(opts, regexp, sedprog)

    import warnings
    warnings.warn("xx-find-grep-sed is obsolete. "
                  "Use xx-filter with --select-grep option instead.")

    try:
        decision_map = xxdiff.xformloop.transform_replace_loop(
            opts, selector, xformer, sys.stdout)
    finally:
        xxdiff.xformloop.postloop_footer(opts)


def main():
    xxdiff.scripts.interruptible_main(findgrepsed_main)

if __name__ == '__main__':
    main()

