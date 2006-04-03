#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
xxdiff-rename [<options>] <from-string> <to-string>

Run a command to perform a replacement of <from-string> to <to-string> in files
and confirm the application of changes via xxdiff.

This program walks a directory hierarchy, selects some files to be processed by
filenames (or by default, process all files found), then the given renaming is
performed on the selected file, compared, if some renaming was applied, a
graphical diff is presented to ask the user whether he wants to apply the
changes or not.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4']


# stdlib imports.
import sys, os, re
from os.path import *

# xxdiff imports.
import xxdiff.scripts
import xxdiff.xformloop
from xxdiff.utils import idify


#-------------------------------------------------------------------------------
#
class RenameTransformer(xxdiff.xformloop.Transformer):
    """
    Transformer that greps the file for a regular expression, and if it matches,
    runs it through a sed command.
    """
    def __init__( self, opts, sfrom, sto ):
        xxdiff.xformloop.Transformer.__init__(self, opts)
        self.sfrom, self.sto = sfrom, sto

        # Escape the string for regexp compilation if necessary.
        if self.opts.regexp:
            regexp = self.sfrom
        else:
            # Escape the special chars
            regexp = re.escape(self.sfrom)
        
        # Compile the regular expression.
        try:
            self.regexp = re.compile(regexp, re.MULTILINE)
        except re.error, e:
            raise SystemExit(
                "Error: Could not compile given regexp '%s':\n%s" % (regexp, e))

    def transform( self, fn, outf ):
        # Open and read input file in memory.
        try:
            inf = file(fn, 'r')
            text = inf.read()
        except IOError, e:
            raise SystemExit("Error: Could not read file '%s':\n  %s" % (fn, e))

        # Replace the string or regexp.
        newtext, nbrepl = self.regexp.subn(self.sto, text)

        # If there were no replacements, skip the file.
        if nbrepl == 0:
            return False
        
        outf.write(newtext)
        return True


#-------------------------------------------------------------------------------
#
def parse_options():
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    parser.add_option('-R', '--regexp', '--re', action='store_true',
                      help="Interpret <from-string> as a regular expression.")

    ## FIXME: TODO, implement this one day.
    ##     parser.add_option('-e', '--emacs-case', '--preserve-case',
    ##                       action='store_true',
    ##                       help="Perform smart replacements with respect to "
    ##                       "case-sensitivity like emacs does.")

    opts, args, selector = xxdiff.xformloop.parse_args(parser)

    # Check that we got two arguments
    if len(args) != 2:
        parser.error("You must specify a <from-string> and a <to-string> to "
                     "rename to.")
    sfrom, sto = args[:2]

    # Force to always perform a diff on output.
    opts.verbose = 2

    # Idify the arguments and setup backup a suffix, to make the default backup
    # directory easier to identify.
    opts.backup_prefix = '%s.%s' % tuple(map(idify, (sfrom, sto)))

    return sfrom, sto, opts, selector


#-------------------------------------------------------------------------------
#
def rename_main():
    """
    Main program for find-grep-sed script.
    """
    # Parse the options.
    sfrom, sto, opts, selector = parse_options()

    # Create an appropriate transformer.
    xformer = RenameTransformer(opts, sfrom, sto)

    try:
        decision_map = xxdiff.xformloop.transform_replace_loop(
            opts, selector, xformer, sys.stdout)
    finally:
        xxdiff.xformloop.postloop_footer(opts)

#-------------------------------------------------------------------------------
#
def main():
    xxdiff.scripts.interruptible_main(rename_main)

if __name__ == '__main__':
    main()

