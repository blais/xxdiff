# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
xx-rename [<options>] <from-str> <to-str> [<from-str> <to-str> ...]

Run a command to perform a replacement of <from-str> to <to-str> in files
and confirm the application of changes via xxdiff.

This program walks a directory hierarchy, selects some files to be processed by
filenames (or by default, process all files found), then the given renaming is
performed on the selected file, compared, if some renaming was applied, a
graphical diff is presented to ask the user whether he wants to apply the
changes or not.

Note that if you specify multiple from/to pairs as arguments, all the renamings
will be performed simultaneously, in the order that they are given.
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


class RenameTransformer(xxdiff.xformloop.Transformer):
    """
    Transformer that greps the file for a regular expression, and if it matches,
    runs it through a sed command.
    """
    def __init__(self, opts, renames):
        """
        'renames' should be a list of (from, to) pairs.
        """
        xxdiff.xformloop.Transformer.__init__(self, opts)

        # Compile the from strings as regexps.
        self.renames = []
        for sfrom, sto in renames:
        
            # Escape the string for regexp compilation if necessary.
            if self.opts.regexp:
                regexp = sfrom
            else:
                # Escape the special chars
                regexp = re.escape(sfrom)

            # Compile the regular expression.
            try:
                refrom = re.compile(regexp, re.MULTILINE)
            except re.error, e:
                raise SystemExit(
                    "Error: Could not compile given regexp '%s':\n%s" %
                    (regexp, e))

            self.renames.append( (refrom, sto) )

    def transform(self, fn, outf):
        # Open and read input file in memory.
        try:
            inf = file(fn, 'r')
            text = inf.read()
        except IOError, e:
            raise SystemExit("Error: Could not read file '%s':\n  %s" % (fn, e))

        # Replace the string or regexp.
        nbrepl = 0
        for refrom, sto in self.renames:
            text, n = refrom.subn(sto, text)
            nbrepl += n

        # If there were no replacements, skip the file.
        if nbrepl == 0:
            return False
        
        outf.write(text)
        return True


def parse_options():
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    parser.add_option('-R', '--regexp', '--re', action='store_true',
                      help="Interpret <from-string> as a regular expression.")

    ## FIXME: TODO, implement this one day, this would be tremendously cool.
    ##     parser.add_option('-e', '--emacs-case', '--preserve-case',
    ##                       action='store_true',
    ##                       help="Perform smart replacements with respect to "
    ##                       "case-sensitivity like emacs does.")

    opts, args, selector = xxdiff.xformloop.parse_args(parser)

    # Check that we got 2, 4, 6, 8, ... arguments
    if len(args) < 2 or len(args) % 2 != 0:
        parser.error("You must specify an even number of <from-string> and "
                     "<to-string> pairs to rename.")
    renames = []
    it = iter(args)
    for sfrom in it:
        renames.append( (sfrom, it.next()) )

    # Force to always perform a diff on output.
    opts.verbose = 2

    # Idify the arguments from the first rename pair and setup a backup suffix,
    # to make the default backup directory easier to identify.
    sfrom, sto = renames[0]

    if opts.backup_prefix is None:
        # Set a better default for the backup prefix.
        opts.backup_prefix = '%s.%s' % tuple(map(idify, (sfrom, sto)))

    return renames, opts, selector


def rename_main():
    """
    Main program for renaming script.
    """
    # Parse the options.
    renames, opts, selector = parse_options()

    # Create an appropriate transformer.
    xformer = RenameTransformer(opts, renames)

    try:
        decision_map = xxdiff.xformloop.transform_replace_loop(
            opts, selector, xformer, sys.stdout)
    finally:
        xxdiff.xformloop.postloop_footer(opts)

def main():
    xxdiff.scripts.interruptible_main(rename_main)

if __name__ == '__main__':
    main()

