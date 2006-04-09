#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions for spawning editor windows.
"""

__author__ = "Martin Blais <blais@furius.ca>"


# stdlib imports.
import sys, os, optparse
from os.path import *
from subprocess import Popen, PIPE
import shutil

# xxdiff imports.
import xxdiff.invoke


__all__ = ('spawn_editor',)


#-------------------------------------------------------------------------------
#

def_editor = ["xterm", "-e" '/usr/bin/vi %s']

#-------------------------------------------------------------------------------
#
def spawn_editor():
    """
    Spawns an editor window and returns a waitable object that will block until
    the editor is done and recuperate the text once it's done.  If you delete
    the returned object without waiting on it, the editor program is killed.
    """
    
    editor = (os.environ.get('XXDIFF_EDITOR', None) or
              os.environ.get('EDITOR', None))
##     if editor:
##         cmd = [editor, 





# FIXME: TODO pre-fill with the list of files
               
# FIXME: add an option so that before starting the review, we run an svn-foreign
# on the directories
