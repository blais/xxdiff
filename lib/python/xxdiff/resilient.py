# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Resilient files.

This is used to manage a location where merge comments and preview history are
saved between sessions.

Support a per-user directory to hold resilient temporary files.  We want to be
able to recycle the files stored in there if the process/script has been
interrupted for some reason--this is quite common for source code, as the
reviewer often realizes he forgot to do something or other, this is a benefit of
reviewing diffs before committing--if the same directories/files are to be
checked-in.  We do not want to store the files in the CWD because that might
change, and it may later interfere with other tools, e.g. show up in 'svn
status' if left behind.  Therefore, we store the comments file in the user's
home directory under a hash computed from the absolute paths of the directories
given as arguments.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, md5
from os.path import *

# xxdiff imports.
from xxdiff.scripts import script_name


resilient_dir = join(os.environ['HOME'], '.%s' % script_name)

def resilient_for_paths(paths):
    """
    Compute a unique file or directory name for a set of paths (considered
    unique as absolute) for a specific process.
    """

    # Compute a unique hash from the given absolute paths.
    comhash = md5.new()
    for p in paths:
        comhash.update(abspath(p))
    resdir = join(resilient_dir, comhash.hexdigest())
    return resdir

def resilient_remove(fn):
    """
    Removes a resilient file, and then checks if the resilient file's parent
    directory can be removed as well.
    """
    os.unlink(fn)
    parent = dirname(fn)
    if not os.listdir(parent):
        os.rmdir(parent)


