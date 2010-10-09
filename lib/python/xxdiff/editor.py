# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions for spawning editor windows.
"""

__author__ = "Martin Blais <blais@furius.ca>"


# stdlib imports.
import os, tempfile
from os.path import *
from subprocess import Popen, PIPE

# xxdiff imports.
from xxdiff.scripts import tmpprefix


__all__ = ('spawn_editor',)


def_editor = ["xterm", "-e", '/usr/bin/vi "%s"']

def spawn_editor(initcontents=None, filename=None):
    """
    Spawns an editor window and returns a waitable object that will block until
    the editor is done and recuperate the text once it's done.  If you delete
    the returned object without waiting on it, the editor program is killed.
    
    If 'initcontents' is given, it is inserted in the temporary file to be
    edited before spawing the editor.  This is only used if no 'filename' has
    been specified.

    If 'filename' is specified, the editor is spawned on that file rather than a
    temporary.  If the file specified by filename exists and is not empty, we
    recycle it.

    This function returns the contents of the edited file, or None, if the edit
    was cancelled.  It may return an empty string.
    """
    # Create and the filename that we will eventually read from.
    tmpf = None
    if filename is not None:
        # Check if the specified file already exists, and don't overwrite it if
        # this is the case.
        if exists(filename) and getsize(filename) > 0:
            tmpf = open(filename, 'r')
            initcontents = None
        else:
            tmpf = open(filename, 'w')
    else:
        tmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)
        filename = tmpf.name
        
    # Initialize the contents of the file if requested.
    if initcontents is not None:
        assert isinstance(initcontents, str)
        tmpf.write(initcontents)
        tmpf.flush()

    # Compute the command to spawn to launch the editor.
    #
    # Note: the editor should the kind to open a new window because we're going
    # to keep printing stuff to stdout during the diffing.  You can just set it
    # up to spawn a new VT if you need to, like in an xterm or something.
    for var in 'XXDIFF_EDITOR', 'SVN_EDITOR', 'EDITOR':
        editor = os.environ.get(var, None)
        if editor:
            break

    shell = False
    if editor:
        if '%s' in editor:
            editor %= filename
            cmd = editor
            shell = True
        else:
            cmd = [editor, filename]
    else:
        cmd = def_editor
        cmd[-1] %= filename

    p = Popen(cmd, shell=shell, stdout=PIPE, stderr=PIPE)
    
    def waiter():
        "Waiter closure."
        stdout, stderr = p.communicate()
        if stderr:
            raise RuntimeError("Error: running editor:\n%s\n" % stderr)
        writtenf = open(tmpf.name, 'r')
        text = writtenf.read()
        writtenf.close()
        tmpf.close()
        return text

    # Set the command string for the client to display if necessary.
    waiter.command = cmd
    if isinstance(waiter.command, (tuple, list)):
        waiter.command = ' '.join(waiter.command)

    return waiter

