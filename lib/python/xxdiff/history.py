# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to record and recall history of actions take on lists of files.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, optparse
from os.path import *

# xxdiff imports.
import xxdiff.resilient
from xxdiff.scripts import script_name
from xxdiff.utils import makedirs


def options_graft(parser):
    """
    Graft options on given parser for invocations of xxdiff.
    """
    group = optparse.OptionGroup(parser, "Options for action history",
                                 "These options affect the history of "
                                 "files/actions.")

    group.add_option('--no-history', action='store_false',
                     dest='history', default=True,
                     help="Do not record to the file history.")

    group.add_option('--clear-history', '--reset-history', action='store_true',
                     help="Clears the file history before starting.")

    group.add_option('--history-file', action='store', metavar='FILENAME',
                     help="Use the specified file to record history.")

    parser.add_option_group(group)

    return group


def options_validate(opts, parser, logs=None):
    """
    Validate xxdiff options.
    """
    if not opts.history and opts.history_file:
        parser.error("You cannot specify a history file and disable history.")


class History(object):
    """
    A class that contains the runtime state of the history of some multi-item
    process/actions.

    The history is saved one file per-line, but you can put whatever you like in
    that one line, for example, the filename, the status of the file, its last
    modification time, etc.  It is basically used as a hash.

    Note: eventually we might want to put more stuff in the history item.  We
    could then consider using a pickle instead.  I have found this unnecessary
    for the current uses of this class (i.e. recording the state of a merge
    preview).

    This is used to record a history of files previewed in a diff loop, so that
    if the viewer loop is interrupted it can be restarted without having to
    review all the past differences.
    """
    def __init__(self, opts, resilient_dir):
        self.history = []
        "Existing history file lines."

        self.histf = None
        """A file object opened in write mode so that we can write new entries
        there."""

        self.opts = opts
        "A reference to the options for use later."
        
        self._initialize(resilient_dir)

    def _initialize(self, resilient_dir):
        """
        Initialize the history module.
        """
        opts = self.opts
        
        # Calculate the location of the history file.
        if opts.history_file:
            histfn = opts.history_file
        else:
            histfn = join(resilient_dir, 'history')

        # Clear the history if requested.
        if opts.clear_history and exists(histfn):
            os.unlink(histfn)

        # Prepare and create a history file if one does not already exist.
        if not opts.history:
            return
        if not exists(histfn):
            makedirs(dirname(histfn), False)
            self.histf = open(histfn, 'w')

        # Otherwise read the history file contents.
        else:
            f = open(histfn, 'r')
            self.history.extend(x.strip() for x  in f.readlines() if x)
            f.close()
            self.histf = open(histfn, 'a')

    def __contains__(self, histitem):
        """
        Support 'in' syntax.
        """
        return self.check(histitem)

    def check(self, histitem):
        """
        Check if the given history item has already been seen.  Call this to
        check every item to be run on your process to be run and to skip it if
        it has already been seen.
        """
        # If history is disabled, check always returns false.
        if not self.opts.history:
            return False

        return histitem in self.history

    def append(self, histitem):
        """
        Append the given history item to the current history and save the file.
        Call this for every succesfully completed item of your process
        """
        # If history is disabled, we don't do anything on append.
        if not self.opts.history:
            return

        # Don't add itmes which are already present in the history.
        if histitem in self.history:
            return

        # Append the new item to the history.
        histitem = histitem.strip()
        self.history.append(histitem)
        
        # Update the history file and make sure that all changes are flushed to
        # disk so that if we get interrupted nothing is lost.
        self.histf.write(histitem)
        self.histf.write('\n')
        self.histf.flush()

    def delete(self):
        """
        Delete the history file.  (This also clears the history.)
        Call this when your process completed succesfully.
        """
        histfn = self.histf.name
        self.histf.close()
        xxdiff.resilient.resilient_remove(histfn)

