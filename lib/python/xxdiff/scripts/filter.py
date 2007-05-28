# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
xx-filter [<options>] <cmd>

Run a command to perform replacements in files and confirm the application of
changes via xxdiff.

This program walks a directory hierarchy, selects some files to be processed by
filenames (or by default, process all files found), then the given filter
command is run with the selected file as input and its output captured,
compared, if the files differ, a graphical diff is presented to ask the user
whether he wants to apply the changes or not.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4']


# stdlib imports.
import sys, os, filecmp
from os.path import *
from subprocess import Popen, PIPE

# xxdiff imports.
import xxdiff.scripts
import xxdiff.xformloop


class FilterTransformer(xxdiff.xformloop.Transformer):
    """
    Transformer that greps the file for a regular expression, and if it matches,
    runs it through a sed command.
    """
    def __init__(self, opts, filtcmd):
        xxdiff.xformloop.Transformer.__init__(self, opts)
        self.filtcmd = filtcmd

    def transform(self, fn, outf):
        # Open input file.
        inf = file(fn, 'r')

        # Run the user's command.
        p = Popen(self.filtcmd, shell=True, stdin=inf, stdout=outf, stderr=PIPE)

        # Note: we do not check the return code, because we do not know the
        # client's convention for the program he invokes.
        returncode = p.wait()

        # Hmmm... what should we do with errors?
        errors = p.stderr.read()

        # Check if the files differ and indicate to process if so.
        #
        # Note: using filecmp should be faster than intercepting and reading all
        # the files in memory, because hopefully they are only reading and
        # comparing chunks at a time to bail out when there is a difference
        # instead of reading the whole data in memory before comparison.
        return not filecmp.cmp(fn, outf.name)


def parse_options():
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    opts, args, selector = xxdiff.xformloop.parse_args(parser)

    # Check that we got two arguments
    if len(args) != 1:
        parser.error("You must specify a filter command to run on the files.")
    filtcmd = args[0]

    # Force to always perform a diff on output.
    opts.verbose = 2

    return filtcmd, opts, selector


def filter_main():
    """
    Main program for find-grep-sed script.
    """
    # Parse the options.
    filtcmd, opts, selector = parse_options()

    # Create an appropriate transformer.
    xformer = FilterTransformer(opts, filtcmd)

    try:
        decision_map = xxdiff.xformloop.transform_replace_loop(
            opts, selector, xformer, sys.stdout)
    finally:
        xxdiff.xformloop.postloop_footer(opts)


def main():
    xxdiff.scripts.interruptible_main(filter_main)

if __name__ == '__main__':
    main()

