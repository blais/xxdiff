# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to transform files via a search loop.  This is meant to be used to
implemnent the filter scripts, and provided as infrastructure for the user to
write his own filtering scripts.
"""

__author__ = "Martin Blais <blais@furius.ca>"


# stdlib imports.
import sys, os, tempfile
from os.path import *

# xxdiff imports.
import xxdiff.scripts
import xxdiff.selectfiles
import xxdiff.backup
import xxdiff.invoke
import xxdiff.checkout
import xxdiff.condrepl
from xxdiff.scripts import tmpprefix


__all__ = ['Transformer', 'parse_args', 'transform_replace_loop']


class Transformer(object):
    """
    Base class for all transformers.
    """
    def __init__(self, opts):
        """
        'opts': an options object -> Option

          We're expecting to find opts.dry_run on it.
        """
        self.opts = opts
        
    def transform(self, fn, outf):
        """
        Process the file 'fn' and write the transformed contents to the
        temporary file 'tmpf' (this file object is already opened in 'w' mode.

        Return True if the file if the transformation was succesful, False if
        the file should be skipped.
        """
        raise NotImplementedError


def parse_args(parser):
    """
    Parse the options and return:
    - an options object -> Options
    - program arguments -> list of strings
    - a selector of the files to iterate over -> generator
    """
    xxmodules = (xxdiff.selectfiles, xxdiff.backup, xxdiff.checkout,
                 xxdiff.invoke, xxdiff.condrepl)
    for mod in xxmodules:
        mod.options_graft(parser)

    xxdiff.scripts.install_autocomplete(parser)

    # Parse arguments
    opts, args = parser.parse_args()

    vargs = opts, parser, sys.stdout
    for mod in xxmodules[1:]:
        mod.options_validate(*vargs)
    selector = xxdiff.selectfiles.options_validate(*vargs)

    return opts, args, selector


def transform_replace_loop(opts, selector, xformer, logs):
    """
    Run a loop through the selected files and conditionally transform them by
    applying the given Transformer and invoking xxdiff to confirm.  Return a map
    of the filenames to the decision code that was used on each of them.
    """
    decision_map = {}
    
    for fn in selector:
        # Create temporary file to receive the transformed file.
        tmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

        # Transform the file.
        if xformer.transform(fn, tmpf) is False:
            # This file is to be skipped by the transformer for some reason.
            decision_map[fn] = 'SKIPPED'
            continue

        # Flush the temporary file before invoking xxdiff on it.
        tmpf.flush()

        # Invoke conditional replacement via xxdiff.
        decision = xxdiff.condrepl.cond_replace(fn, tmpf.name, opts, logs)
        decision_map[fn] = decision

    return decision_map


def postloop_footer(opts, decision_map=None):
    """
    Print some common stuff at the end of the looping scripts.
    You don't have to add this to your script, but it's convenient.
    """
    xxdiff.backup.print_reminder(opts)

